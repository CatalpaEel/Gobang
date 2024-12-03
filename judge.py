import subprocess
import json
import time

n=15

time_all=[]

board=[['-' for i in range(n)] for j in range(n)]

black_message = {"requests":[{"x":-1,"y":-1}],"responses":[]}
white_message = {"requests":[],"responses":[]}

for i in range(20):
    t=time.time()

    result = subprocess.run("game.exe", input=json.dumps(black_message), capture_output=True, text=True)
    result = result.stdout

    result = json.loads(result)

    if "response" == None:
        break
    put = result["response"]
    black_message["responses"].append(put)
    white_message["requests"].append(put)
    board[result["response"]["x"]][result["response"]["y"]] = 'O'

    delta_time=time.time()-t
    time_all.append(delta_time)
    print(2*i-1,"time:",delta_time);

    print(result)
    for j in range(15):
        print(''.join(''.join(board[j])))

    
    t=time.time()
    result = subprocess.run("game.exe", input=json.dumps(white_message), capture_output=True, text=True)
    result = result.stdout
    result = json.loads(result)

    if result == None:
        break

    put = result["response"]
    white_message["responses"].append(put)
    black_message["requests"].append(put)
    board[result["response"]["x"]][result["response"]["y"]] = 'X'

    delta_time=time.time()-t
    time_all.append(delta_time)
    print(2*i,"time:",delta_time);
    print(result)
    for j in range(15):
        print(''.join(''.join(board[j])))

