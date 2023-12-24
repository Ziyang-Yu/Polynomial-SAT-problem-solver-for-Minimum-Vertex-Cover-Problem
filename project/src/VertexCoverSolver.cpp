#include <chrono>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <unistd.h>

#include "Graph.h"
#include "VertexCoverSolver.h"

pthread_cond_t pthread_cond_cnf_sat = PTHREAD_COND_INITIALIZER;
pthread_mutex_t pthread_mutex_cnf_sat = PTHREAD_MUTEX_INITIALIZER;

typedef struct thread_payload {
    std::shared_ptr<Graph> graph;
    std::vector<int> result;
    bool isTimeout;
    int timeSpentInMicroseconds;
} thread_payload_t;

VertexCoverSolver::VertexCoverSolver() {
    this->state = 0;
    this->nVertex = 0;
    this->timeoutInSeconds = 120;
    this->parser = std::unique_ptr<Parser>(new Parser());
}

int getMicroseconds(timespec ts_start, timespec ts_end) {
    return (ts_end.tv_sec - ts_start.tv_sec) * 1000 * 1000 + (ts_end.tv_nsec - ts_start.tv_nsec) / 1000;
}

std::string VertexCoverSolver::AcceptLine(std::string line) {
    // removing heading and tailing spaces
    unsigned cmdStart = 0;
    while (cmdStart < line.size() && line[cmdStart] == ' ') {
        cmdStart++;
    }
    if (cmdStart == line.size()) {
        // empty line or only spaces
        return "";
    }
    unsigned cmdEnd = line.size() - 1;
    while (cmdEnd > cmdStart && line[cmdEnd] == ' ') {
        cmdEnd--;
    }
    line = line.substr(cmdStart, cmdEnd - cmdStart + 1);

    if (state == 0) {
        // Accepting "V ..."
        state = 1;
        this->nVertex = 0;
        nVertex = this->parser->GetVertexCount(line);
        return "";
    }
    else {
        // Accepting "E ..."
        state = 0;
        if (nVertex < 2) {
            throw std::invalid_argument("V is already invalid, thus invalidating E");
        }

        auto edges = this->parser->GetEdges(line, nVertex);
        auto graph = std::shared_ptr<Graph>(new Graph(nVertex, edges));

        // Create threads and run with different methods
        pthread_t thread_approx_1, thread_approx_2, thread_cnf_sat;
        thread_payload_t thread_payload_approx_1 = {
            .graph = graph,
            .result = std::vector<int>(),
            .isTimeout = false,
            .timeSpentInMicroseconds = 0
        };
        thread_payload_t thread_payload_approx_2 = {
            .graph = graph,
            .result = std::vector<int>(),
            .isTimeout = false,
            .timeSpentInMicroseconds = 0
        };

        // Manually new this instance to hold a reference for graph shared_ptr so that the graph remains in heap after the function stack is cleaned up
        thread_payload_t *thread_payload_cnf_sat = new thread_payload_t {
            .graph = graph,
            .result = std::vector<int>(),
            .isTimeout = false,
            .timeSpentInMicroseconds = 0
        };

        auto thread_func_approx_1 = [](void *data) -> void* {
            thread_payload_t *payload =(thread_payload_t*)data;

            clockid_t cid;
            struct timespec ts_start, ts_end;
            pthread_getcpuclockid(pthread_self(), &cid);

            clock_gettime(cid, &ts_start);
            payload->graph->getVertexCoverWithAPPROX_VC_1(payload->result);
            clock_gettime(cid, &ts_end);

            payload->timeSpentInMicroseconds = getMicroseconds(ts_start, ts_end);
            return nullptr;
        };

        auto thread_func_approx_2 = [](void *data) -> void* {
            thread_payload_t *payload =(thread_payload_t*)data;

            clockid_t cid;
            struct timespec ts_start, ts_end;
            pthread_getcpuclockid(pthread_self(), &cid);

            clock_gettime(cid, &ts_start);
            payload->graph->getVertexCoverWithAPPROX_VC_2(payload->result);
            clock_gettime(cid, &ts_end);

            payload->timeSpentInMicroseconds = getMicroseconds(ts_start, ts_end);
            return nullptr;
        };

        auto thread_func_cnf_sat = [](void *data) -> void* {
            thread_payload_t *payload =(thread_payload_t*)data;

            clockid_t cid;
            struct timespec ts_start, ts_end;
            pthread_getcpuclockid(pthread_self(), &cid);

            clock_gettime(cid, &ts_start);
            auto result = payload->graph->GetMinimalVertexCover_CNF_SAT();
            clock_gettime(cid, &ts_end);

            pthread_mutex_lock(&pthread_mutex_cnf_sat);

            if (payload->isTimeout) {
                delete payload;
            } else {
                payload->result = result;
                payload->timeSpentInMicroseconds = getMicroseconds(ts_start, ts_end);
                pthread_cond_signal(&pthread_cond_cnf_sat);
            }

            pthread_mutex_unlock(&pthread_mutex_cnf_sat);

            return nullptr;
        };

        int ret = 0;
        ret = pthread_create(&thread_approx_1, nullptr, thread_func_approx_1, &thread_payload_approx_1);
        if (ret != 0) {
            throw std::runtime_error("error starting approx_1 thread");
        }
        ret = pthread_create(&thread_approx_2, nullptr, thread_func_approx_2, &thread_payload_approx_2);
        if (ret != 0) {
            throw std::runtime_error("error starting approx_2 thread");
        }

        pthread_mutex_lock(&pthread_mutex_cnf_sat);

        ret = pthread_create(&thread_cnf_sat, nullptr, thread_func_cnf_sat, thread_payload_cnf_sat);
        if (ret != 0) {
            throw std::runtime_error("error starting cnf_sat thread");
        }

        timespec wait_until_cnf_sat = {
            .tv_sec = this->timeoutInSeconds,
            .tv_nsec = 0
        };

        auto currentTime = std::chrono::system_clock::now();
        wait_until_cnf_sat.tv_sec += std::chrono::system_clock::to_time_t(currentTime);

        if (0 != pthread_cond_timedwait(&pthread_cond_cnf_sat, &pthread_mutex_cnf_sat, &wait_until_cnf_sat)) {
            thread_payload_cnf_sat->isTimeout = true;
            pthread_detach(thread_cnf_sat);
        }

        pthread_mutex_unlock(&pthread_mutex_cnf_sat);

        pthread_join(thread_approx_1, NULL);
        pthread_join(thread_approx_2, NULL);
        if (!thread_payload_cnf_sat->isTimeout) {
            pthread_join(thread_cnf_sat, NULL);
        }

        // Construct result
        std::stringstream ss = std::stringstream();

        ss << "CNF-SAT-VC: ";
        if (thread_payload_cnf_sat->isTimeout) {
            // payload is to be deleted in the detached thread when timeout happens so the graph remains in heap during its solving process
            ss << "timeout";
        } else{
            this->appendResult(ss, thread_payload_cnf_sat->result);
            delete thread_payload_cnf_sat;
        }
        ss << std::endl;

        ss << "APPROX-VC-1: ";
        this->appendResult(ss, thread_payload_approx_1.result);
        ss << std::endl;

        ss << "APPROX-VC-2: ";
        this->appendResult(ss, thread_payload_approx_2.result);
        ss << std::endl;

        // Uncomment to print performance data
        /*
        if (!thread_payload_cnf_sat->isTimeout) {
            std::cout << "CNF-SAT-VC(microsecond): " << thread_payload_cnf_sat->timeSpentInMicroseconds << std::endl;
        }
        std::cout << "APPROX-VC-1(microsecond): " << thread_payload_approx_1.timeSpentInMicroseconds << std::endl;
        std::cout << "APPROX-VC-2(microsecond): " << thread_payload_approx_2.timeSpentInMicroseconds << std::endl;
        std::cout << "APPROX-VC-1(size): " << thread_payload_approx_1.result.size() << std::endl;
        std::cout << "APPROX-VC-2(size): " << thread_payload_approx_2.result.size() << std::endl;
        if (!thread_payload_cnf_sat->isTimeout) {
            std::cout << "CNF-SAT-VC(size): " << thread_payload_cnf_sat->result.size() << std::endl;
            std::cout << "APPROX-VC-1(ratio): " << (float)thread_payload_approx_1.result.size() / thread_payload_cnf_sat->result.size() << std::endl;
            std::cout << "APPROX-VC-2(ratio): " << (float)thread_payload_approx_2.result.size() / thread_payload_cnf_sat->result.size() << std::endl;
        }
        */

        return ss.str();
    }
}

void VertexCoverSolver::appendResult(std::stringstream &ss, const std::vector<int> &result) {
    if (result.size() > 0) {
        ss << result[0];
        for (unsigned i = 1; i < result.size(); i++) {
            ss << "," << result[i];
        }
    }
}
