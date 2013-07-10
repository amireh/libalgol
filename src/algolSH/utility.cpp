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

#include <vector>
#include <string>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

typedef std::string string_t;

struct command_t {
  string_t      name;
};

static std::vector<command_t> commands_;

static char* dupstr(const char*);
static void* xmalloc(int);
static char* gen_cmd(const char* text, int state);

char** my_completion( const char * text , int start, int) {
  char **matches;

  matches = (char **)NULL;

  if (start == 0) {
    matches = rl_completion_matches ((char*)text, gen_cmd);
  }

  return (matches);
}

static char* gen_cmd(const char* text, int state)
{
    static size_t list_index, len;
    const char *name;
    command_t *cmd = nullptr;

    if (!state) {
        list_index = 0;
        len = strlen (text);
    }

    if (!commands_.empty()) {
      while (list_index < commands_.size()) {
        cmd = &commands_[list_index++];
        name = cmd->name.c_str();
        if (strncmp (name, text, len) == 0) {
          return (dupstr(name));
        }
      }
    }

    /* If no names matched, then return NULL. */
    return ((char *)NULL);
}

static char* dupstr(const char* s) {
  char *r;

  r = (char*) xmalloc ((strlen (s) + 1));
  strcpy (r, s);
  return (r);
}

static void* xmalloc(int size)
{
    void *buf;

    buf = malloc (size);
    if (!buf) {
        fprintf (stderr, "Error: Out of memory. Exiting.'n");
        exit (1);
    }

    return buf;
}

void add_command(const char* cmd) {
  commands_.push_back({ string_t(cmd) });
}