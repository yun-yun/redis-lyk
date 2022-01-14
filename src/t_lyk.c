//
// Created by 58 on 2022/1/14.
//

#include "server.h"
#include <math.h> /* isnan(), isinf() */

void lykCommand(client *c) {
//    lykGenericCommand(c);
    char *hello;
    char buf[] = "hello, i'm lyk";
    hello = buf;
    robj *o = createStringObject(hello, 14);


    addReplyBulk(c, o);
}