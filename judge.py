import subprocess
import json

board=[['-' for i in range(16)] for j in range(16)]

black_message = {"requests":[{"x":-1,"y":-1}],"responses":[]}
white_message = {"requests":[],"responses":[]}

for i in range(20):

    result = subprocess.run("game.exe", input=json.dumps(black_message), capture_output=True, text=True)
    result = result.stdout

    result = json.loads(result)
    put = result["response"]
    black_message["responses"].append(put)
    white_message["requests"].append(put)
    board[result["response"]["x"]][result["response"]["y"]] = 'X'

    print(result)
    for j in range(1,16):
        print(''.join(''.join(board[j][1:])))

    result = subprocess.run("game.exe", input=json.dumps(white_message), capture_output=True, text=True)
    result = result.stdout
    result = json.loads(result)
    put = result["response"]
    white_message["responses"].append(put)
    black_message["requests"].append(put)
    board[result["response"]["x"]][result["response"]["y"]] = 'O'

    print(result)