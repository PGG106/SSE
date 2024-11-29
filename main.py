import subprocess,os
p=None
def chess_bot(obs):
    global p
    f="./Alexandria" if os.path.exists("./Alexandria") else "/kaggle_simulations/agent/Alexandria"
    if not p or p.poll()!=None:
        p=subprocess.Popen(f,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=1)
    t=obs.remainingOverageTime*1000
    # Treat delay as inc
    p.stdin.write(f"position fen {obs.board}\ngo wtime {t} btime {t} winc 70 binc 70\n")
    p.stdin.flush()
    line = ""
    while True:
        line = p.stdout.readline()
        if line != "\n":
            break
    ponder_move = line.split()[-1]
    best_move = line.split()[-2]
    return best_move