
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "json.h"

#define argv0 (program_invocation_name)

struct zebu_value* get(
	struct zebu_value* dict,
	const char* field)
{
	assert(dict->dict);
	
	for (unsigned i = 0, n = dict->entries.n; i < n; i++)
		if (!strcmp(dict->entries.data[i]->key, field))
			return dict->entries.data[i]->value;
	
	return NULL;
}

int main()
{
	FILE* stream = fopen("./problemset.json", "r");
	
	if (!stream)
	{
		fprintf(stderr, "%s: fopen(\"problemset.json\"): %m\n", argv0);
		printf("remember to run ./curl.sh first!\n");
		exit(1);
	}
	
	char* home = getenv("HOME");
	
	if (!home)
	{
		fprintf(stderr, "%s: getenv(HOME): %m\n", argv0);
		exit(1);
	}
	
	if (chdir(home) < 0)
	{
		fprintf(stderr, "%s: chdir(\"%s\"): %m\n", argv0, home);
		exit(1);
	}
	
	if (mkdir("icpc", 0774) < 0 && errno != EEXIST)
	{
		fprintf(stderr, "%s: mkdir(\"icpc\"): %m\n", argv0);
		exit(1);
	}
	
	if (chdir("icpc") < 0)
	{
		fprintf(stderr, "%s: chdir(\"%s\"): %m\n", argv0, "icpc");
		exit(1);
	}
	
	if (mkdir("contests", 0774) < 0 && errno != EEXIST)
	{
		fprintf(stderr, "%s: mkdir(\"contests\"): %m\n", argv0);
		exit(1);
	}
	
	if (chdir("contests") < 0)
	{
		fprintf(stderr, "%s: chdir(\"%s\"): %m\n", argv0, "contests");
		exit(1);
	}
	
	struct zebu_$start* start = zebu_parse(stream);
	
	struct zebu_value* result = get(start->value, "\"result\"");
	
	struct zebu_value* problems = get(result, "\"problems\"");
	
	assert(problems->list);
	
	for (unsigned i = 0, n = problems->elements.n; i < n; i++)
	{
		struct zebu_value* problem = problems->elements.data[i];
		
		struct zebu_value* contestId = get(problem, "\"contestId\"");
		
		struct zebu_value* index = get(problem, "\"index\"");
		
		assert(contestId->integer);
		assert(index->string);
		
		char index_char = index->string[1];
		
		// printf("processing '%s/%c'...\n", contestId->integer, index_char);
		
		if (mkdir(contestId->integer, 0774) < 0 && errno != EEXIST)
		{
			fprintf(stderr, "%s: mkdir(\"%s\"): %m\n", argv0, contestId->integer);
			exit(1);
		}
		
		char path[PATH_MAX];
		
		sprintf(path, "%s/%c", contestId->integer, index_char);
		
		if (mkdir(path, 0774) < 0 && errno != EEXIST)
		{
			fprintf(stderr, "%s: mkdir(\"%s\"): %m\n", argv0, contestId->integer);
			exit(1);
		}
		
		sprintf(path, "%s/%c/url", contestId->integer, index_char);
		
		char url[PATH_MAX];
		sprintf(url, "https://codeforces.com/problemset/problem/%s/%c", contestId->integer, index_char);
		
		if (symlink(url, path) < 0 && errno != EEXIST)
		{
			fprintf(stderr, "%s: symlink(\"%s\", \"%s\"): %m\n", argv0, url, path);
			exit(1);
		}
	}
	
	fclose(stream);
	
	return 0;
}

























