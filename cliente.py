import socket
import sys
import datetime
import ssl
import json
import os
import re

CACHE_FILE = 'http_cache.json'

def load_cache():
    """Carga la caché desde un archivo JSON si existe. Retorna un diccionario vacío si el archivo no existe."""
    if os.path.exists(CACHE_FILE):
        with open(CACHE_FILE, 'r', encoding='utf-8') as file:
            return json.load(file)
    return {}

def save_cache(cache):
    """Guarda el estado actual de la caché en un archivo JSON."""
    with open(CACHE_FILE, 'w', encoding='utf-8') as file:
        json.dump(cache, file)

def flush_cache():
    """Elimina todos los datos almacenados en la caché borrando el archivo."""
    if os.path.exists(CACHE_FILE):
        os.remove(CACHE_FILE)
    print("La caché ha sido limpiada.")

def parse_content_type(response_headers):
    """Extrae el tipo de contenido de los encabezados HTTP de la respuesta."""
    for line in response_headers.split("\r\n"):
        if line.lower().startswith("content-type"):
            return line.split(":", 1)[1].strip()
    return ""

def parse_url(url):
    """Analiza una URL y extrae sus componentes como protocolo, host, puerto y path."""
    parsed_url = {}
    parts = url.split('//')
    if len(parts) == 2:
        protocol, url = parts
        parsed_url['protocol'] = protocol[:-1]
    else:
        parsed_url['protocol'] = 'http'
    if '/' in url:
        url, path = url.split('/', 1)
        parsed_url['path'] = '/' + path
    else:
        parsed_url['path'] = '/'
    if ':' in url:
        url, port = url.split(':', 1)
        parsed_url['port'] = int(port)
    else:
        parsed_url['port'] = 80 if parsed_url.get('protocol', 'http') == 'http' else 443
    parsed_url['host'] = url
    return parsed_url

def send_http_request(url, method, data=None, timeout=10):
    """Envía una solicitud HTTP/HTTPS al servidor especificado por la URL con el método y datos proporcionados."""
    parsed_url = parse_url(url)
    # Se construye la línea de solicitud y los encabezados HTTP
    request_line = f"{method} {parsed_url['path']} HTTP/1.1"
    headers = [f"Host: {parsed_url['host']}", "Connection: close"]
    if method == 'POST' and data:
        headers.extend([f"Content-Length: {len(data)}", "Content-Type: application/x-www-form-urlencoded"])
    request = f"{request_line}\r\n" + "\r\n".join(headers) + "\r\n\r\n" + (data if data else "")
  
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(timeout)
        if parsed_url['protocol'] == 'https':  
            context = ssl.create_default_context()
            client_socket = context.wrap_socket(sock, server_hostname=parsed_url['host'])
        else:
            client_socket = sock
        client_socket.connect((parsed_url['host'], parsed_url['port']))
        client_socket.sendall(request.encode())
        response = b""
       
        while True:
            part = client_socket.recv(4096)
            if not part:
                break
            response += part
        client_socket.close()
        return request_line, response
    except Exception as e:
        print(f"Error al enviar la solicitud HTTP: {e}")
        return None, None

def sanitize_filename(filename):
    """Sanitiza el nombre de archivo para asegurar que es válido y seguro para su uso en el sistema de archivos."""
    filename = re.sub(r'[<>:"/\\|?*]', "", filename)
    filename = filename[:255]
    if not filename:
        filename = "index.html"
    return filename

def decode_percent_encoding(s):
    """Decodifica caracteres codificados en porcentaje en una cadena."""
    result = ''
    i = 0
    while i < len(s):
        if s[i] == '%' and i + 2 < len(s):
            hex_value = s[i+1:i+3]
            result += chr(int(hex_value, 16))
            i += 3
        else:
            result += s[i]
            i += 1
    return result

def save_resource(response, url):
    """Guarda el cuerpo de la respuesta en un archivo, utilizando el tipo de contenido para determinar el modo de escritura.
    Si el contenido es de tipo HTML, también lo imprime en la consola."""
    headers, _, body = response.partition(b"\r\n\r\n")
    content_type = parse_content_type(headers.decode("utf-8", "ignore"))
    path = parse_url(url)['path']
    filename = sanitize_filename(decode_percent_encoding(path.split("/")[-1]))
    mode = "w" if "text" in content_type or "json" in content_type else "wb"
    content = body.decode("utf-8", "ignore") if mode == "w" else body
    try:
        with open(filename, mode, encoding="utf-8" if mode == "w" else None) as file:
            file.write(content)
    except FileNotFoundError as e:
        print(f"Error al intentar guardar el recurso: {e}")
        return None, None

    """Si el archivo es texto html lo imprime por consola"""
    if 'html' in content_type:
        print("Contenido HTML recibido:")
        print(content)

    return filename, content_type

def save_response_to_file(request, response, log_file, filename, content_type):
    """Guarda un registro de la solicitud y respuesta HTTP en un archivo de log."""
    now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    log_entry = f"{now} HTTP Request: {request} Filename: {filename} ContentType: {content_type} HTTP Response: {response}\n"
    with open(log_file, "a", encoding='utf-8') as log:
        log.write(log_entry)

def main():
    """Función principal que maneja la lógica del cliente HTTP, incluyendo el manejo de caché y la ejecución de solicitudes."""
    cache = load_cache()
    if len(sys.argv) == 2 and sys.argv[1].lower() == "flush":
        flush_cache()
    elif len(sys.argv) < 4:
        print("Uso correcto: ./httpclient </path/log.log> <url> <method> [<data>] | flush")
        sys.exit(1)
    else:
        log_file, url, method = sys.argv[1:4]
        data = sys.argv[4] if len(sys.argv) > 4 else None
        if url in cache:
            print("Respuesta cargada desde caché.")
            response = cache[url]['response']
            if isinstance(response, str):
                response = response.encode('utf-8')
            else:
                response = bytes(response)
        else:
            print("Enviando solicitud a la red.")
            _, response = send_http_request(url, method, data)
            if response:
                headers, _, body = response.partition(b"\r\n\r\n")
                content_type = parse_content_type(headers.decode("utf-8", "ignore"))
                if "text" in content_type or "json" in content_type:
                    cache[url] = {'response': body.decode('utf-8', errors='ignore')}
                else:
                    cache[url] = {'response': list(body)}
                save_cache(cache)
        if response:
            filename, content_type = save_resource(response, url)
            if filename and content_type:
                save_response_to_file(method + " " + url, response.decode('utf-8', 'ignore') if "text" in content_type or "json" in content_type else 'BINARY CONTENT', log_file, filename, content_type)
                print(f"Respuesta guardada en {filename}.")

if __name__ == "__main__":
    main()
