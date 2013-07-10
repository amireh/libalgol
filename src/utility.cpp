/* libalgol - a collection of plug-ins for developing back-end C++ web tools
 * Copyright (c) 2013 Algol Labs, LLC.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stddef.h>
#include "algol/utility.hpp"

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <libxml/HTMLparser.h>

#define UNICODE_MAX 0x10FFFFul

namespace algol {
namespace utility {

  static
  int __html_encode(unsigned char* out, int *outlen, const unsigned char* in, int *inlen);

  static
  size_t __html_decode(char *dest, const char *src);

  static const char *named_entities[][2] =
  {
    { "AElig;", "Æ" },
    { "Aacute;", "Á" },
    { "Acirc;", "Â" },
    { "Agrave;", "À" },
    { "Alpha;", "Α" },
    { "Aring;", "Å" },
    { "Atilde;", "Ã" },
    { "Auml;", "Ä" },
    { "Beta;", "Β" },
    { "Ccedil;", "Ç" },
    { "Chi;", "Χ" },
    { "Dagger;", "‡" },
    { "Delta;", "Δ" },
    { "ETH;", "Ð" },
    { "Eacute;", "É" },
    { "Ecirc;", "Ê" },
    { "Egrave;", "È" },
    { "Epsilon;", "Ε" },
    { "Eta;", "Η" },
    { "Euml;", "Ë" },
    { "Gamma;", "Γ" },
    { "Iacute;", "Í" },
    { "Icirc;", "Î" },
    { "Igrave;", "Ì" },
    { "Iota;", "Ι" },
    { "Iuml;", "Ï" },
    { "Kappa;", "Κ" },
    { "Lambda;", "Λ" },
    { "Mu;", "Μ" },
    { "Ntilde;", "Ñ" },
    { "Nu;", "Ν" },
    { "OElig;", "Œ" },
    { "Oacute;", "Ó" },
    { "Ocirc;", "Ô" },
    { "Ograve;", "Ò" },
    { "Omega;", "Ω" },
    { "Omicron;", "Ο" },
    { "Oslash;", "Ø" },
    { "Otilde;", "Õ" },
    { "Ouml;", "Ö" },
    { "Phi;", "Φ" },
    { "Pi;", "Π" },
    { "Prime;", "″" },
    { "Psi;", "Ψ" },
    { "Rho;", "Ρ" },
    { "Scaron;", "Š" },
    { "Sigma;", "Σ" },
    { "THORN;", "Þ" },
    { "Tau;", "Τ" },
    { "Theta;", "Θ" },
    { "Uacute;", "Ú" },
    { "Ucirc;", "Û" },
    { "Ugrave;", "Ù" },
    { "Upsilon;", "Υ" },
    { "Uuml;", "Ü" },
    { "Xi;", "Ξ" },
    { "Yacute;", "Ý" },
    { "Yuml;", "Ÿ" },
    { "Zeta;", "Ζ" },
    { "aacute;", "á" },
    { "acirc;", "â" },
    { "acute;", "´" },
    { "aelig;", "æ" },
    { "agrave;", "à" },
    { "alefsym;", "ℵ" },
    { "alpha;", "α" },
    { "amp;", "&" },
    { "and;", "∧" },
    { "ang;", "∠" },
    { "apos;", "'" },
    { "aring;", "å" },
    { "asymp;", "≈" },
    { "atilde;", "ã" },
    { "auml;", "ä" },
    { "bdquo;", "„" },
    { "beta;", "β" },
    { "brvbar;", "¦" },
    { "bull;", "•" },
    { "cap;", "∩" },
    { "ccedil;", "ç" },
    { "cedil;", "¸" },
    { "cent;", "¢" },
    { "chi;", "χ" },
    { "circ;", "ˆ" },
    { "clubs;", "♣" },
    { "cong;", "≅" },
    { "copy;", "©" },
    { "crarr;", "↵" },
    { "cup;", "∪" },
    { "curren;", "¤" },
    { "dArr;", "⇓" },
    { "dagger;", "†" },
    { "darr;", "↓" },
    { "deg;", "°" },
    { "delta;", "δ" },
    { "diams;", "♦" },
    { "divide;", "÷" },
    { "eacute;", "é" },
    { "ecirc;", "ê" },
    { "egrave;", "è" },
    { "empty;", "∅" },
    { "emsp;", " " },
    { "ensp;", " " },
    { "epsilon;", "ε" },
    { "equiv;", "≡" },
    { "eta;", "η" },
    { "eth;", "ð" },
    { "euml;", "ë" },
    { "euro;", "€" },
    { "exist;", "∃" },
    { "fnof;", "ƒ" },
    { "forall;", "∀" },
    { "frac12;", "½" },
    { "frac14;", "¼" },
    { "frac34;", "¾" },
    { "frasl;", "⁄" },
    { "gamma;", "γ" },
    { "ge;", "≥" },
    { "gt;", ">" },
    { "hArr;", "⇔" },
    { "harr;", "↔" },
    { "hearts;", "♥" },
    { "hellip;", "…" },
    { "iacute;", "í" },
    { "icirc;", "î" },
    { "iexcl;", "¡" },
    { "igrave;", "ì" },
    { "image;", "ℑ" },
    { "infin;", "∞" },
    { "int;", "∫" },
    { "iota;", "ι" },
    { "iquest;", "¿" },
    { "isin;", "∈" },
    { "iuml;", "ï" },
    { "kappa;", "κ" },
    { "lArr;", "⇐" },
    { "lambda;", "λ" },
    { "lang;", "〈" },
    { "laquo;", "«" },
    { "larr;", "←" },
    { "lceil;", "⌈" },
    { "ldquo;", "“" },
    { "le;", "≤" },
    { "lfloor;", "⌊" },
    { "lowast;", "∗" },
    { "loz;", "◊" },
    { "lrm;", "\xE2\x80\x8E" },
    { "lsaquo;", "‹" },
    { "lsquo;", "‘" },
    { "lt;", "<" },
    { "macr;", "¯" },
    { "mdash;", "—" },
    { "micro;", "µ" },
    { "middot;", "·" },
    { "minus;", "−" },
    { "mu;", "μ" },
    { "nabla;", "∇" },
    { "nbsp;", " " },
    { "ndash;", "–" },
    { "ne;", "≠" },
    { "ni;", "∋" },
    { "not;", "¬" },
    { "notin;", "∉" },
    { "nsub;", "⊄" },
    { "ntilde;", "ñ" },
    { "nu;", "ν" },
    { "oacute;", "ó" },
    { "ocirc;", "ô" },
    { "oelig;", "œ" },
    { "ograve;", "ò" },
    { "oline;", "‾" },
    { "omega;", "ω" },
    { "omicron;", "ο" },
    { "oplus;", "⊕" },
    { "or;", "∨" },
    { "ordf;", "ª" },
    { "ordm;", "º" },
    { "oslash;", "ø" },
    { "otilde;", "õ" },
    { "otimes;", "⊗" },
    { "ouml;", "ö" },
    { "para;", "¶" },
    { "part;", "∂" },
    { "permil;", "‰" },
    { "perp;", "⊥" },
    { "phi;", "φ" },
    { "pi;", "π" },
    { "piv;", "ϖ" },
    { "plusmn;", "±" },
    { "pound;", "£" },
    { "prime;", "′" },
    { "prod;", "∏" },
    { "prop;", "∝" },
    { "psi;", "ψ" },
    { "quot;", "\"" },
    { "rArr;", "⇒" },
    { "radic;", "√" },
    { "rang;", "〉" },
    { "raquo;", "»" },
    { "rarr;", "→" },
    { "rceil;", "⌉" },
    { "rdquo;", "”" },
    { "real;", "ℜ" },
    { "reg;", "®" },
    { "rfloor;", "⌋" },
    { "rho;", "ρ" },
    { "rlm;", "\xE2\x80\x8F" },
    { "rsaquo;", "›" },
    { "rsquo;", "’" },
    { "sbquo;", "‚" },
    { "scaron;", "š" },
    { "sdot;", "⋅" },
    { "sect;", "§" },
    { "shy;", "\xC2\xAD" },
    { "sigma;", "σ" },
    { "sigmaf;", "ς" },
    { "sim;", "∼" },
    { "spades;", "♠" },
    { "sub;", "⊂" },
    { "sube;", "⊆" },
    { "sum;", "∑" },
    { "sup;", "⊃" },
    { "sup1;", "¹" },
    { "sup2;", "²" },
    { "sup3;", "³" },
    { "supe;", "⊇" },
    { "szlig;", "ß" },
    { "tau;", "τ" },
    { "there4;", "∴" },
    { "theta;", "θ" },
    { "thetasym;", "ϑ" },
    { "thinsp;", " " },
    { "thorn;", "þ" },
    { "tilde;", "˜" },
    { "times;", "×" },
    { "trade;", "™" },
    { "uArr;", "⇑" },
    { "uacute;", "ú" },
    { "uarr;", "↑" },
    { "ucirc;", "û" },
    { "ugrave;", "ù" },
    { "uml;", "¨" },
    { "upsih;", "ϒ" },
    { "upsilon;", "υ" },
    { "uuml;", "ü" },
    { "weierp;", "℘" },
    { "xi;", "ξ" },
    { "yacute;", "ý" },
    { "yen;", "¥" },
    { "yuml;", "ÿ" },
    { "zeta;", "ζ" },
    { "zwj;", "\xE2\x80\x8D" },
    { "zwnj;", "\xE2\x80\x8C" }
  };

  static int cmp(const void *key, const void *element)
  {
    return strncmp((const char *)key, *(const char **)element,
      strlen(*(const char **)element));
  }

  static const char *get_named_entity(const char *name)
  {
    const char **entity = (const char**)bsearch(name, named_entities,
      sizeof(named_entities) / sizeof(*named_entities),
      sizeof(*named_entities), cmp);

    return entity ? entity[1] : NULL;
  }

  static size_t putc_utf8(unsigned long cp, char *buffer)
  {
    unsigned char *bytes = (unsigned char *)buffer;

    if(cp <= 0x007Ful)
    {
      bytes[0] = (unsigned char)cp;
      return 1;
    }

    if(cp <= 0x07FFul)
    {
      bytes[1] = (unsigned char)((2u << 6) | (cp & 0x3Fu));
      bytes[0] = (unsigned char)((6u << 5) | (cp >> 6));
      return 2;
    }

    if(cp <= 0xFFFFul)
    {
      bytes[2] = (unsigned char)(( 2u << 6) | ( cp       & 0x3Fu));
      bytes[1] = (unsigned char)(( 2u << 6) | ((cp >> 6) & 0x3Fu));
      bytes[0] = (unsigned char)((14u << 4) |  (cp >> 12));
      return 3;
    }

    if(cp <= 0x10FFFFul)
    {
      bytes[3] = (unsigned char)(( 2u << 6) | ( cp        & 0x3Fu));
      bytes[2] = (unsigned char)(( 2u << 6) | ((cp >>  6) & 0x3Fu));
      bytes[1] = (unsigned char)(( 2u << 6) | ((cp >> 12) & 0x3Fu));
      bytes[0] = (unsigned char)((30u << 3) |  (cp >> 18));
      return 4;
    }

    return 0;
  }

  static int parse_entity(const char *current, char **to,
    const char **from)
  {
    const char *end = strchr(current, ';');
    if(!end) return 0;

    if(current[1] == '#')
    {
      char *tail = NULL;
      errno = 0;

      int hex = current[2] == 'x' || current[2] == 'X';

      unsigned long cp = strtoul(
        current + (hex ? 3 : 2), &tail, hex ? 16 : 10);

      if(tail == end && !errno && cp <= UNICODE_MAX)
      {
        *to += putc_utf8(cp, *to);
        *from = end + 1;

        return 1;
      }
    }
    else
    {
      const char *entity = get_named_entity(&current[1]);
      if(entity)
      {
        size_t len = strlen(entity);
        memcpy(*to, entity, len);

        *to += len;
        *from = end + 1;

        return 1;
      }
    }

    return 0;
  }

  static size_t __html_decode(char *dest, const char *src)
  {
    if(!src) src = dest;

    char *to = dest;
    const char *from = src;

    const char *current;
    while((current = strchr(from, '&')))
    {
      memcpy(to, from, (size_t)(current - from));
      to += current - from;

      if(parse_entity(current, &to, &from))
        continue;

      from = current;
      *to++ = *from++;
    }

    size_t remaining = strlen(from);

    memcpy(to, from, remaining);
    to += remaining;

    *to = 0;
    return (size_t)(to - dest);
  }

  static int __html_encode(unsigned char* out, int *outlen, const unsigned char* in, int *inlen)
  {
    const unsigned char* processed = in;
    const unsigned char* outend;
    const unsigned char* outstart = out;
    const unsigned char* instart = in;
    const unsigned char* inend;
    unsigned int c, d;
    int trailing;

    if ((out == NULL) || (outlen == NULL) || (inlen == NULL) || (in == NULL))
      return(-1);

    outend = out + (*outlen);
    inend = in + (*inlen);
    while (in < inend) {
      d = *in++;
      if      (d < 0x80)  { c= d; trailing= 0; }
      else if (d < 0xC0) {
        /* trailing byte in leading position */
        *outlen = out - outstart;
        *inlen = processed - instart;
        return(-2);
      }
      else if (d < 0xE0)  { c= d & 0x1F; trailing= 1; }
      else if (d < 0xF0)  { c= d & 0x0F; trailing= 2; }
      else if (d < 0xF8)  { c= d & 0x07; trailing= 3; }
      else {
        /* no chance for this in Ascii */
        *outlen = out - outstart;
        *inlen = processed - instart;
        return(-2);
      }

      if (inend - in < trailing)
          break;

      while (trailing--) {
        if (((d= *in++) & 0xC0) != 0x80) {
          *outlen = out - outstart;
          *inlen = processed - instart;
          return(-2);
        }
        c <<= 6;
        c |= d & 0x3F;
      }

      /* assertion: c is a single UTF-4 value */
      if ((c < 0x80) && (c != (unsigned int) '"') && (c != (unsigned int) '\'') &&
          (c != '&') && (c != '<') && (c != '>')) {
          if (out >= outend)
        break;
          *out++ = c;
      }
      else {
       const htmlEntityDesc * ent;
       const char *cp;
        char nbuf[16];
        int len;

        /*
         * Try to lookup a predefined HTML entity for it
         */
        ent = htmlEntityValueLookup(c);
        if (ent == NULL) {
          snprintf(nbuf, sizeof(nbuf), "#%u", c);
          cp = nbuf;
        }
        else {
          cp = ent->name;
        }

        len = strlen(cp);

        if (out + 2 + len > outend)
          break;

        *out++ = '&';
        memcpy(out, cp, len);
        out += len;
        *out++ = ';';
      }

      processed = in;
    }

    *outlen = out - outstart;
    *inlen = processed - instart;

    return(0);
  }


  bool html_encode(string_t& dest, const string_t& src)
  {
    /* count how many characters we need to escape, estimating for speed needs */
    int insz = src.size();
    int count = 0;
    int i = 0;
    for (i = 0; i < insz; i++ ) {
      if (!isalnum(src[i]))
        count++;
    }

    int outsz = insz + (count * 6) + 1;
    unsigned char *out = new unsigned char[outsz];
    int res = __html_encode(out, &outsz, (const unsigned char*)src.c_str(), &insz);

    if (res == 0) {
      dest = string_t((const char*)out, outsz);
    }

    delete[] out;
    return res == 0;
  }

  bool html_decode(string_t &dest, const string_t& src)
  {
    char *out = NULL;
    __html_decode(out, src.c_str());
    dest = string_t(out);
    return true;
  }

  string_t get_hostname() {
    struct addrinfo hints, *info;
    int gai_result;

    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
      exit(1);
    }

    string_t out(info->ai_canonname);
    freeaddrinfo(info);

    if (out == "localhost" || out == "127.0.0.1") {
      return string_t(hostname);
    }

    return out;
  }

  const char* get_hostname_cstr() {
    return get_hostname().c_str();
  }
}
}
