//
// Created by 58 on 2022/1/14.
//

#include "server.h"
#include <math.h> /* isnan(), isinf() */

void lykCommand(client *c) {
//    lykGenericCommand(c);
    char *name = c->argv[1]->ptr;
    size_t len = sdslen(name);
    serverLog(LL_WARNING, "lyk :" + len);

    char *hello;
    char buf[] = "hello, i'm lyk";
    hello = buf;
    robj *o = createStringObject(hello, sizeof(buf) - 1);


    addReplyBulk(c, o);
}