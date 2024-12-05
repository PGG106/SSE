#include "time_manager.h"

#include "search.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

// Calculate how much time to spend on searching a move
void Optimum(struct SearchInfo* info, int time, int inc) {
    // Reserve some time overhead to avoid timing out in the engine-gui communication process
    const int safety_overhead = min(100, time / 2);
    time -= safety_overhead;
    // else if we received wtime/btime we calculate an over and upper bound for the time usage based on fixed coefficients
    if (info->timeset) {
        int basetime = time * 0.054 + inc * 0.85;
        // Never use more than 76% of the total time left for a single move
        const double maxtimeBound = 0.76 * time;
        // optime is the time we use to stop if we just cleared a depth
        const double optime = min(0.76 * basetime, maxtimeBound);
        // maxtime is the absolute maximum time we can spend on a search (unless it is bigger than the bound)
        const double maxtime = min(3.04 * basetime, maxtimeBound);
        info->stoptimeMax = info->starttime + maxtime;
        info->stoptimeBaseOpt = optime;
        info->stoptimeOpt = info->starttime + info->stoptimeBaseOpt;
    }
}

bool StopEarly(const struct SearchInfo* info) {
    // check if we used all the nodes/movetime we had or if we used more than our lowerbound of time
    return (info->timeset) && GetTimeMs() > info->stoptimeOpt;
}

bool TimeOver(const struct SearchInfo* info) {
    // check if more than Maxtime passed and we have to stop
    return  ((info->timeset) && ((info->nodes & 1023) == 1023)
        && GetTimeMs() > info->stoptimeMax);
}

void ScaleTm(struct ThreadData* td) {
    const int bestmove = return_bestmove;
    // Calculate how many nodes were spent on checking the best move
    const double bestMoveNodesFraction = (double)(td->nodeSpentTable[FromTo(bestmove)]) / (double)(td->info.nodes);
    const double nodeScalingFactor = (1.52 - bestMoveNodesFraction) * 1.74;
    // Scale the search time based on how many nodes we spent and how the best move changed
    td->info.stoptimeOpt = min(td->info.starttime + td->info.stoptimeBaseOpt * nodeScalingFactor, td->info.stoptimeMax);
}