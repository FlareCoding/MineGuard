import json

class config_reader:
    def __init__(self, path) -> None:
        with open(path) as file:
            self.__data = json.load(file)

    def get(self, key):
        return self.__data[key]
