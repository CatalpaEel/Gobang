import subprocess
import json

n=15

board=[['-' for i in range(n)] for j in range(n)]

black_message = {"requests":[{"x":-1,"y":-1}],"responses":[]}
white_message = {"requests":[],"responses":[]}

for i in range(20):

    print(black_message)
    result = subprocess.run("game.exe", input=json.dumps(black_message), capture_output=True, text=True)
    result = result.stdout

    result = json.loads(result)
    put = result["response"]
    black_message["responses"].append(put)
    white_message["requests"].append(put)
    board[result["response"]["x"]][result["response"]["y"]] = 'O'

    print(result)
    for j in range(15):
        print(''.join(''.join(board[j])))

    print(white_message)

    result = subprocess.run("game.exe", input=json.dumps(white_message), capture_output=True, text=True)
    result = result.stdout
    result = json.loads(result)
    put = result["response"]
    white_message["responses"].append(put)
    black_message["requests"].append(put)
    board[result["response"]["x"]][result["response"]["y"]] = 'X'

    print(result)
    for j in range(15):
        print(''.join(''.join(board[j])))