/*
 * Copyright (c) 2009-2021, Redis Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __SCRIPT_H_
#define __SCRIPT_H_

/*
 * Script.c unit provides an API for functions and eval 
 * to interact with Redis. Interaction includes mostly
 * executing commands, but also functionalities like calling
 * Redis back on long scripts or check if the script was killed.
 *
 * The interaction is done using a scriptRunCtx object that
 * need to be created by the user and initialized using scriptPrepareForRun.
 *
 * Detailed list of functionalities expose by the unit:
 * 1. Calling commands (including all the validation checks such as
 *    acl, cluster, read only run, ...)
 * 2. Set Resp
 * 3. Set Replication method (AOF/REPLICATION/NONE)
 * 4. Call Redis back to on long running scripts to allow Redis reply
 *    to clients and perform script kill
 */

/*
 * scriptInterrupt function will return one of those value,
 *
 * - SCRIPT_KILL - kill the current running script.
 * - SCRIPT_CONTINUE - keep running the current script.
 */
#define SCRIPT_KILL 1
#define SCRIPT_CONTINUE 2

/* runCtx flags */
#define SCRIPT_WRITE_DIRTY            (1ULL<<0) /* indicate that the current script already performed a write command */
#define SCRIPT_RANDOM_DIRTY           (1ULL<<1) /* indicate that the current script already performed a random reply command.
                                                   Thanks to this flag we'll raise an error every time a write command
                                                   is called after a random command and prevent none deterministic
                                                   replication or AOF. */
#define SCRIPT_MULTI_EMMITED          (1ULL<<2) /* indicate that we already wrote a multi command to replication/aof */
#define SCRIPT_TIMEDOUT               (1ULL<<3) /* indicate that the current script timedout */
#define SCRIPT_KILLED                 (1ULL<<4) /* indicate that the current script was marked to be killed */
#define SCRIPT_READ_ONLY              (1ULL<<5) /* indicate that the current script should only perform read commands */
#define SCRIPT_EVAL_REPLICATION       (1ULL<<6) /* mode for eval, indicate that we replicate the
                                                   script invocation and not the effects */
#define SCRIPT_EVAL_MODE              (1ULL<<7) /* Indicate that the current script called from legacy Lua */
typedef struct scriptRunCtx scriptRunCtx;

struct scriptRunCtx {
    const char *funcname;
    client *c;
    client *original_client;
    int flags;
    int repl_flags;
    monotime start_time;
    mstime_t snapshot_time;
};

void scriptPrepareForRun(scriptRunCtx *r_ctx, client *engine_client, client *caller, const char *funcname);
void scriptResetRun(scriptRunCtx *r_ctx);
int scriptSetResp(scriptRunCtx *r_ctx, int resp);
int scriptSetRepl(scriptRunCtx *r_ctx, int repl);
void scriptCall(scriptRunCtx *r_ctx, robj **argv, int argc, sds *err);
int scriptInterrupt(scriptRunCtx *r_ctx);
void scriptKill(client *c, int is_eval);
int scriptIsRunning();
const char* scriptCurrFunction();
int scriptIsEval();
int scriptIsTimedout();
client* scriptGetClient();
client* scriptGetCaller();
mstime_t scriptTimeSnapshot();
long long scriptRunDuration();

#endif /* __SCRIPT_H_ */
