#Holi Lindos :)
import socket
import sys
import datetime
import ssl
import json
import os
from urllib.parse import urlparse, unquote


# Este es el archivo donde vamos a guardar la caché.NOSE Y HAY QUE PREGUNTAR SI COMO EL VA Y MIRA EN LA CACHE PUES NO VUELVE A CARGAR PERO PUES PUEDE CAMBIAR 
#SI NO ES TEXTO LA SOLICITUD NO SE GUARDA EN CACHE
CACHE_FILE = 'http_cache.json'

def load_cache():
    # Primero, intentamos cargar la caché desde un archivo JSON.
    # Si no existe, simplemente devolvemos un diccionario vacío.
    if os.path.exists(CACHE_FILE):
        with open(CACHE_FILE, 'r', encoding='utf-8') as file:
            return json.load(file)
    return {}

def save_cache(cache):
    # Aquí guardamos el estado actual de nuestra caché en un archivo.
    # Esto nos permite persistir datos entre ejecuciones.
    with open(CACHE_FILE, 'w', encoding='utf-8') as file:
        json.dump(cache, file)

def flush_cache():
    # Esta función nos permite limpiar toda la caché,
    # simplemente eliminando el archivo.
    if os.path.exists(CACHE_FILE):
        os.remove(CACHE_FILE)
    print("La caché ha sido limpiada.")

def parse_content_type(response_headers):
    # Buscamos en los encabezados de la respuesta el tipo de contenido.
    # Esto es útil para saber cómo debemos manejar la respuesta.
    for line in response_headers.split("\r\n"):
        if line.lower().startswith("content-type"):
            return line.split(":", 1)[1].strip()
    return ""

def parse_url(url): #Funcion para no utilizar librerias y parsearlo manualmente
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
    parsed_url = parse_url(url)
    host = parsed_url['host']
    port = parsed_url['port']
    path = parsed_url['path']
    request_line = f"{method} {path} HTTP/1.1"
    headers = [f"Host: {host}", "Connection: close"]
    if method == 'POST' and data:
        headers.extend([f"Content-Length: {len(data)}", "Content-Type: application/x-www-form-urlencoded"])
    request = f"{request_line}\r\n" + "\r\n".join(headers) + "\r\n\r\n" + (data if data else "")
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(timeout)
        if parsed_url['protocol'] == 'https':  
            context = ssl.create_default_context()
            client_socket = context.wrap_socket(sock, server_hostname=host)
        else:
            client_socket = sock
        client_socket.connect((host, port))
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

def save_resource(response, url):
    # Dependiendo del tipo de contenido, guardamos la respuesta
    # en un archivo apropiado, ya sea texto o binario.
    headers, _, body = response.partition(b"\r\n\r\n")
    content_type = parse_content_type(headers.decode("utf-8", "ignore"))
    filename = unquote(urlparse(url).path.split("/")[-1]) or "index.html"
    if "text" in content_type or "json" in content_type:
        mode = "w"
        content = body.decode("utf-8", "ignore")
    else:
        mode = "wb"
        content = body
    with open(filename, mode, encoding="utf-8" if mode == "w" else None) as file:
        file.write(content)
    return filename, content_type

def save_response_to_file(request, response, log_file, filename, content_type):
    # Registramos cada respuesta recibida en un archivo de log.
    # Esto es útil para depuración o para llevar un registro.
    now = datetime.datetime.now()
    date_time = now.strftime("%Y-%m-%d %H:%M:%S")
    log_entry = f"{date_time} HTTP Request: {request} Filename: {filename} ContentType: {content_type} HTTP Response: {response}\n"
    with open(log_file, "a", encoding='utf-8') as log:
        log.write(log_entry)

def main():
    # La función principal coordina todo el flujo del programa,
    # desde manejar la caché hasta procesar los argumentos de línea de comandos.
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
            response = cache[url]['response'].encode('utf-8')
        else:
            print("Enviando solicitud a la red.")
            _, response = send_http_request(url, method, data)
            if response:
                headers, _, body = response.partition(b"\r\n\r\n")
                content_type = parse_content_type(headers.decode("utf-8", "ignore"))
                if "text" in content_type or "json" in content_type:
                    cache[url] = {'response': body.decode('utf-8')}
                    save_cache(cache)
        if response:
            filename, content_type = save_resource(response, url)
            save_response_to_file(method + " " + url, response.decode('utf-8', 'ignore'), log_file, filename, content_type)
            print(f"Respuesta guardada en {filename}.")

if __name__ == "__main__":
    main()

# By Santix
