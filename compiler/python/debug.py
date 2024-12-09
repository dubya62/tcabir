
DEBUG = 1

def dbg(message:str):
    if DEBUG:
        print(message)

def panic(message:str):
    print(message)
    exit(1)



