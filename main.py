import subprocess as sp ,os
p=None
def chess_bot(obs):
    global p
    f= "/kaggle_simulations/agent/Alexandria"
    if not p or p.poll()!=None:
        p=sp.Popen(f,stdin=sp.PIPE,stdout=sp.PIPE,stderr=sp.PIPE,text=1)
    t=obs.remainingOverageTime*1000
    p.stdin.write(f"stop\n")
    p.stdin.flush()
    p.stdin.write(f"position fen {obs.board}\ngo wtime {t} btime {t} winc 70 binc 70\n")
    p.stdin.flush()
    line = ""
    while True:
        line = p.stdout.readline()
        if line != "\n":
            break
    ponder_move = line.split()[-1]
    best_move = line.split()[-2]
    p.stdin.write(f"ponder {ponder_move}\n")
    p.stdin.flush()
    return best_move