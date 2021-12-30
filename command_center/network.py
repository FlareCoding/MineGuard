import socket

class command_client:
    def __init__(self, config) -> None:
        self.__server_ip = config.get('server-ip')
        self.__server_port = config.get('server-port')
        self.__socket = None

    def connect(self) -> bool:
        try:
            self.__socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.__socket.settimeout(5)
            self.__socket.connect((self.__server_ip, self.__server_port))
        except socket.error:
            return False

        return True

    def get_server_ip(self) -> str:
        return self.__server_ip

    def get_server_port(self) -> int:
        return self.__server_port

    def send_cmd(self, cmd) -> bool:
        try:
            self.__socket.send(cmd.encode('utf-8'))
        except socket.error:
            return False

        return True

    def receive_response(self) -> tuple:
        response = ''
        success = True
        try:
            response = self.__socket.recv(4096).decode('utf-8')
        except socket.error:
            success = False

        return (success, response)
