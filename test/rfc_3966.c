/* URIPathFinder: A simple parser for URIs
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Nate Bragg
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "rfc_3966.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>

static int failures = 0;

#define NULL_CHECK_P(id) (result.pars.id && !p_##id) || (!result.pars.id && p_##id)
#define BAD_LEN_CHECK_P(id, len) result.pars.id && len != (int)strlen(p_##id)
#define BAD_COMPARE_P(id) result.pars.id && strncmp(result.pars.id, p_##id, strlen(p_##id))
#define NULL_CHECK(id) (result.id && !p_##id) || (!result.id && p_##id)
#define BAD_LEN_CHECK(id, len) result.id && len != (int)strlen(p_##id)
#define BAD_COMPARE(id) result.id && strncmp(result.id, p_##id, strlen(p_##id))
void test_tel(char *p_url, char *p_global_number, char *p_local_number, char *p_ext, char *p_isdn, char *p_context, char *p_pars_1, char *p_pars_2, char *p_pars_3, char *p_pars_4)
{
    Tel result = parse_telephone(p_url);
    int global_len  = len_global_number(&result);
    int local_len   = len_local_number(&result);
    int ext_len     = len_par_ext(&result);
    int isdn_len    = len_par_isdn(&result);
    int context_len = len_par_context(&result);
    int pars_1_len  = len_par_pars_1(&result);
    int pars_2_len  = len_par_pars_2(&result);
    int pars_3_len  = len_par_pars_3(&result);
    int pars_4_len  = len_par_pars_4(&result);
    if (NULL_CHECK(global_number) || BAD_LEN_CHECK(global_number, global_len) || BAD_COMPARE(global_number) ||
        NULL_CHECK(local_number)  || BAD_LEN_CHECK(local_number, local_len)  || BAD_COMPARE(local_number)  ||
        NULL_CHECK_P(ext)         || BAD_LEN_CHECK_P(ext,     ext_len)        || BAD_COMPARE_P(ext)         ||
        NULL_CHECK_P(isdn)        || BAD_LEN_CHECK_P(isdn,    isdn_len)       || BAD_COMPARE_P(isdn)        ||
        NULL_CHECK_P(context)     || BAD_LEN_CHECK_P(context, context_len)    || BAD_COMPARE_P(context)     ||
        NULL_CHECK_P(pars_1)      || BAD_LEN_CHECK_P(pars_1,  pars_1_len)     || BAD_COMPARE_P(pars_1)      ||
        NULL_CHECK_P(pars_2)      || BAD_LEN_CHECK_P(pars_2,  pars_2_len)     || BAD_COMPARE_P(pars_2)      ||
        NULL_CHECK_P(pars_3)      || BAD_LEN_CHECK_P(pars_3,  pars_3_len)     || BAD_COMPARE_P(pars_3)      ||
        NULL_CHECK_P(pars_4)      || BAD_LEN_CHECK_P(pars_4,  pars_4_len)     || BAD_COMPARE_P(pars_4)) {
        printf("Failed for URI: %s\n", p_url);
        printf("Expected - global_number: %s, local_number: %s, ext: %s, isdn: %s, context: %s, pars_1: %s, pars_2: %s, pars_3: %s, pars_4: %s\n",
               p_global_number ? p_global_number : "NULL",
               p_local_number ? p_local_number : "NULL",
               p_ext ? p_ext : "NULL",
               p_isdn ? p_isdn : "NULL",
               p_context ? p_context : "NULL",
               p_pars_1 ? p_pars_1 : "NULL",
               p_pars_2 ? p_pars_2 : "NULL",
               p_pars_3 ? p_pars_3 : "NULL",
               p_pars_4 ? p_pars_4 : "NULL");
        printf("Output   - global_number: %.*s, local_number: %.*s, ext: %.*s, isdn: %.*s, context: %.*s, pars_1: %.*s, pars_2: %.*s, pars_3: %.*s, pars_4: %.*s\n",
               result.global_number ? global_len  : 4, result.global_number ? result.global_number : "NULL",
               result.local_number  ? local_len   : 4, result.local_number  ? result.local_number  : "NULL",
               result.pars.ext      ? ext_len     : 4, result.pars.ext      ? result.pars.ext      : "NULL",
               result.pars.isdn     ? isdn_len    : 4, result.pars.isdn     ? result.pars.isdn     : "NULL",
               result.pars.context  ? context_len : 4, result.pars.context  ? result.pars.context  : "NULL",
               result.pars.pars_1   ? pars_1_len  : 4, result.pars.pars_1   ? result.pars.pars_1   : "NULL",
               result.pars.pars_2   ? pars_2_len  : 4, result.pars.pars_2   ? result.pars.pars_2   : "NULL",
               result.pars.pars_3   ? pars_3_len  : 4, result.pars.pars_3   ? result.pars.pars_3   : "NULL",
               result.pars.pars_4   ? pars_4_len  : 4, result.pars.pars_4   ? result.pars.pars_4   : "NULL");
        failures++;
    }
}

int main()
{
    /* Valid URIs */
    test_tel("tel:+1234567890", "+1234567890", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    test_tel("tel:+1234567890;ext=1234", "+1234567890", NULL, ";ext=1234", NULL, NULL, NULL, NULL, NULL, NULL);
    test_tel("tel:+1234567890;isub=5678", "+1234567890", NULL, NULL, ";isub=5678", NULL, NULL, NULL, NULL, NULL);
    test_tel("tel:1234567890;phone-context=+1-800", NULL, "1234567890", NULL, NULL, ";phone-context=+1-800", NULL, NULL, NULL, NULL);
    test_tel("tel:555-1234;phone-context=+1-800", NULL, "555-1234", NULL, NULL, ";phone-context=+1-800", NULL, NULL, NULL, NULL);
    test_tel("tel:+1(234)567-890;ext=123;foo=bar", "+1(234)567-890", NULL, ";ext=123", NULL, NULL, ";foo=bar", NULL, NULL, NULL);
    test_tel("tel:+1234567890;isub=5678;ext=2345", "+1234567890", NULL, ";ext=2345", ";isub=5678", NULL, NULL, NULL, NULL, NULL);
    test_tel("tel:+1234567890;param1=value1;ext=789", "+1234567890", NULL, ";ext=789", NULL, NULL, ";param1=value1", NULL, NULL, NULL);
    test_tel("tel:555-1212;phone-context=example.com;param1=value1;ext=5555", NULL, "555-1212", ";ext=5555", NULL, ";phone-context=example.com", ";param1=value1", NULL, NULL, NULL);
    test_tel("tel:+1234567890;ext=789;param1=value1;isub=456", "+1234567890", NULL, ";ext=789", ";isub=456", NULL, ";param1=value1", NULL, NULL, NULL);
    test_tel("tel:+14155552671;foo=bar$;isub=100", "+14155552671", NULL, NULL, ";isub=100", NULL, ";foo=bar$", NULL, NULL, NULL);
    test_tel("tel:19876543210;param1=abc;param2=def;param3=ghi;ext=234;param4=jkl;param5=mno;param6=pqr;param7=stu;param8=vwx;isub=789;param9=yz;param10=012;param11=345;phone-context=local;param12=678", NULL, "19876543210", ";ext=234", ";isub=789", ";phone-context=local", ";param1=abc;param2=def;param3=ghi", ";param4=jkl;param5=mno;param6=pqr;param7=stu;param8=vwx", ";param9=yz;param10=012;param11=345", ";param12=678");
    test_tel("tel:+14155552671;isub=abcd;custom1=123;ext=001;custom2=xyz", "+14155552671", NULL, ";ext=001", ";isub=abcd", NULL, ";custom1=123", ";custom2=xyz", NULL, NULL);
    test_tel("tel:+1234567890;foo=bar;isub=100;ext=555", "+1234567890", NULL, ";ext=555", ";isub=100", NULL, ";foo=bar", NULL, NULL, NULL);
    test_tel("tel:5551234567;phone-context=local;param1=val1;param2=val2", NULL, "5551234567", NULL, NULL, ";phone-context=local", ";param1=val1;param2=val2", NULL, NULL, NULL);
    test_tel("tel:5551234567;foo=bar;ext=123;isub=9999;phone-context=example.com", NULL, "5551234567", ";ext=123", ";isub=9999", ";phone-context=example.com", ";foo=bar", NULL, NULL, NULL);
    test_tel("tel:555-1111;phone-context=+44;param=abc;ext=12", NULL, "555-1111", ";ext=12", NULL, ";phone-context=+44", ";param=abc", NULL, NULL, NULL);
    test_tel("tel:+1-800-555-0199", "+1-800-555-0199", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    test_tel("tel:+44.20.7946.0958;ext=123", "+44.20.7946.0958", NULL, ";ext=123", NULL, NULL, NULL, NULL, NULL, NULL);
    test_tel("tel:+81-3-1234-5678;param1=foo&bar;ext=789", "+81-3-1234-5678", NULL, ";ext=789", NULL, NULL, ";param1=foo&bar", NULL, NULL, NULL);
    test_tel("tel:+91-22-30456789;isub=12345", "+91-22-30456789", NULL, NULL, ";isub=12345", NULL, NULL, NULL, NULL, NULL);
    test_tel("tel:+64-4-555-1234;isub=backup", "+64-4-555-1234", NULL, NULL, ";isub=backup", NULL, NULL, NULL, NULL, NULL);
    test_tel("tel:+49-30-123456;isub=ISDN-1", "+49-30-123456", NULL, NULL, ";isub=ISDN-1", NULL, NULL, NULL, NULL, NULL);
    test_tel("tel:(123)456-7890;phone-context=+1-800-555-5555", NULL, "(123)456-7890", NULL, NULL, ";phone-context=+1-800-555-5555", NULL, NULL, NULL, NULL);
    test_tel("tel:123-456-7890;phone-context=global.example.com", NULL, "123-456-7890", NULL, NULL, ";phone-context=global.example.com", NULL, NULL, NULL, NULL);
    test_tel("tel:+86-10-1234-5678;param-extra=value_12", "+86-10-1234-5678", NULL, NULL, NULL, NULL, ";param-extra=value_12", NULL, NULL, NULL);
    test_tel("tel:+49-89-555-6789;ext=99;param=val$ue", "+49-89-555-6789", NULL, ";ext=99", NULL, NULL, ";param=val$ue", NULL, NULL, NULL);
    test_tel("tel:+33-1-555-9999;isub=test%20data;param=xyz!", "+33-1-555-9999", NULL, NULL, ";isub=test%20data", NULL, ";param=xyz!", NULL, NULL, NULL);
    test_tel("tel:(212)-555-1212;phone-context=example.net;param=name%22value", NULL, "(212)-555-1212", NULL, NULL, ";phone-context=example.net", ";param=name%22value", NULL, NULL, NULL);
    test_tel("tel:+81-3-1234-5678;param=alpha~beta", "+81-3-1234-5678", NULL, NULL, NULL, NULL, ";param=alpha~beta", NULL, NULL, NULL);
    test_tel("tel:+351-21-555-4321;isub=meta!key;param=foo/bar", "+351-21-555-4321", NULL, NULL, ";isub=meta!key", NULL, ";param=foo/bar", NULL, NULL, NULL);
    test_tel("tel:+34-91-555-1212;param=name:value", "+34-91-555-1212", NULL, NULL, NULL, NULL, ";param=name:value", NULL, NULL, NULL);
    test_tel("tel:+972-3-456-7890;param=123/456;isub=id$123", "+972-3-456-7890", NULL, NULL, ";isub=id$123", NULL, ";param=123/456", NULL, NULL, NULL);
    test_tel("tel:(404)-555-6789;phone-context=local.example;ext=300-200", NULL, "(404)-555-6789", ";ext=300-200", NULL, ";phone-context=local.example", NULL, NULL, NULL, NULL);
    test_tel("tel:+39-06-1234-5678;isub=check-this;param=name123", "+39-06-1234-5678", NULL, NULL, ";isub=check-this", NULL, ";param=name123", NULL, NULL, NULL);
    test_tel("tel:555-987-6543;phone-context=context.test", NULL, "555-987-6543", NULL, NULL, ";phone-context=context.test", NULL, NULL, NULL, NULL);
    test_tel("tel:+7-495-123-4567;param=data+name;isub=code*789", "+7-495-123-4567", NULL, NULL, ";isub=code*789", NULL, ";param=data+name", NULL, NULL, NULL);
    test_tel("tel:(555)-555-5555;phone-context=sample.data", NULL, "(555)-555-5555", NULL, NULL, ";phone-context=sample.data", NULL, NULL, NULL, NULL);
    test_tel("tel:+1-212-123-4567;param=attr$value", "+1-212-123-4567", NULL, NULL, NULL, NULL, ";param=attr$value", NULL, NULL, NULL);
    test_tel("tel:+49-30-555-4321;isub=meta@key", "+49-30-555-4321", NULL, NULL, ";isub=meta@key", NULL, NULL, NULL, NULL, NULL);
    test_tel("tel:+41-44-555-1212;isub=meta!id;param=*&123", "+41-44-555-1212", NULL, NULL, ";isub=meta!id", NULL, ";param=*&123", NULL, NULL, NULL);

    /* Invalid URIs */
    /* * local number can't have alphas */
    test_tel("tel:123abc7890", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    /* * local number can't have ! */
    test_tel("tel:+123-456-78!90", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    /* * global number can't have phone-context */
    test_tel("tel:+5551234567;foo=bar;phone-context=example.com", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    /* * local number missing phone-context */
    test_tel("tel:1234567890", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    /* * local number with other parameters but missing phone-context */
    test_tel("tel:19876543210;ext=234", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    /* * phone-context missing value */
    test_tel("tel:5551234567;foo=bar;phone-context=;isub=9999", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    /* * phone-context should start with + or alpha */
    test_tel("tel:5551234;ext=23;isub=567;phone-context=800", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    /* * parameters can't have spaces inside value */
    test_tel("tel:5551234567;ext=12 phone-context=example.com", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    /* * global number can't have spaces */
    test_tel("tel:+1 800 555 5555", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    /* * global number with other parameters can't have spaces */
    test_tel("tel:+1415 555 2671;foo=bar", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    /* * parameter values can't start with spaces */
    test_tel("tel:+5551234567;foo= bar;isub=9999", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    printf("Total failures: %d\n", failures);
    return 0;
}
