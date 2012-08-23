#include <v8.h>
#include <node.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <node_buffer.h>
#include <string.h>

using namespace std;
using namespace node;
using namespace v8;

/* parameters check */
#define REQ_STR_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsString()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be a String"))); \
Local<Object> VAR = Local<Object>::Cast(args[I]);
#define REQ_NUM_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsNumber()) \
return ThrowException(Exception::TypeError( \
String::New("Argument " #I " must be a String"))); \
Local<Object> VAR = Local<Object>::Cast(args[I]);
/* endian swap */
#define ___swab16(x)
{
    __u16 __x = (x);
    ((__u16)(
        (((__u16)(__x) & (__u16)0x00ffU) << 8) |
        (((__u16)(__x) & (__u16)0xff00U) >> 8) ));
}
#define ___swab32(x)
{
    __u32 __x = (x);
    ((__u32)(
            (((__u32)(__x) & (__u32)0x000000ffUL) << 24) |
            (((__u32)(__x) & (__u32)0x0000ff00UL) <<  8) |
            (((__u32)(__x) & (__u32)0x00ff0000UL) >>  8) |
            (((__u32)(__x) & (__u32)0xff000000UL) >> 24) ));
}

/* 1 is big-endian , 0 is small-endian */
int checkCPUEndian()
{
       union{
            unsigned long int i;
            unsigned char s[4];
       }c;
       c.i = 0x12345678;
       return (0x12 == c.s[0]);
}


class Main: ObjectWrap
{
private:
    int m_count;
public:
    static Persistent<FunctionTemplate> s_ct;
    static void Init(Handle<Object> target)
    {
        HandleScope scope;

        Local<FunctionTemplate> t = FunctionTemplate::New(New);

        s_ct = Persistent<FunctionTemplate>::New(t);
        s_ct->InstanceTemplate()->SetInternalFieldCount(1);
        s_ct->SetClassName(String::NewSymbol("Main"));

        NODE_SET_PROTOTYPE_METHOD(s_ct, "ipToLong", ipToLong);
        NODE_SET_PROTOTYPE_METHOD(s_ct, "longToIp", longToIp);
        NODE_SET_PROTOTYPE_METHOD(s_ct, "isBigEndian", isBigEndian);
        NODE_SET_PROTOTYPE_METHOD(s_ct, "swapEndian", swapEndian);

        target->Set(String::NewSymbol("Main"),
                    s_ct->GetFunction());
    }

    Main() :
        m_count(0)
    {
    }

    ~Main()
    {
    }

    static Handle<Value> ipToLong(const Arguments& args)
    {
        HandleScope scope;
        REQ_STR_ARG(0, ip_str);

        unsigned long ip[4] = {0};
        sscanf(*ip_str, "%ld.%ld.%ld.%ld", &ip[0], &ip[1], &ip[2], &ip[3]);
        if (ip[0] === NULL || ip[1] === NULL || ip[2] === NULL || ip[3] ===NULL) {
            return scope.Close(Number::New(0));
        }
        unsigned long address = 0;
        address |= ip[3] << 24;
        address |= ip[2] << 16;
        address |= ip[1] << 8;
        address |= ip[0];
        return scope.Close(v8::UInt32::New(address));
    }

    static Handle<Value> longToIp(const Arguments& args)
    {
        HandleScope scope;
        REQ_NUM_ARG(0, ip_l);
        long id = (long) ip_l::Value();
        long ip[4] = {0};
        ip[0] = ((id & 0xff));
        ip[1] = (((id >> 8) & 0xff));
        ip[2] = (((id >> 16) & 0xff));
        ip[3] = (((id >> 24) & 0xff));
        char address[30] = {0};
        sprintf(address, "%lf.%lf.%lf.%lf", ip[0], ip[1], ip[2], ip[3]);
        return scope.Close(v8::String::New(address));
    }
    static Handle<Value> swapEndian(const Arguments& args)
    {
        HandleScope scope;
        REQ_NUM_ARG(0, orgi);
        long orgl = (long) orgi::Value();
        return scope.Close(v8::Number::New(___swab32(orgl)));
    }
    static Handle<Value> isBigEndian(const Arguments& args)
    {
        HandleScope scope;
        int ret = checkCPUEndian();
        return scope.Close(v8::Boolean::New(ret === 1));
        
    }

}