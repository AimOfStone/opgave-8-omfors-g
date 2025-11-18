#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

// #include "include/Franktext.h"

#define MAX_WORD_COUNT 50000
#define MAX_SUCCESSOR_COUNT MAX_WORD_COUNT / 10

char book[] ={


#embed "pg84.txt", '\0'};        //embed doesn't work. Can't test my code.

/// Array of tokens registered so far
/// No duplicates are allowed.
char *tokens[MAX_WORD_COUNT];
/// `tokens`'s current size
size_t tokens_size = 0;

/// Array of successor tokens
/// One token can have many successor tokens. `succs[x]` corresponds to
/// `token[x]`'s successors.
/// We store directly tokens instead of token_ids, because we will directly
/// print them. If we wanted to delete the book, then it would make more sense
/// to store `token_id`s
char *succs[MAX_WORD_COUNT][MAX_SUCCESSOR_COUNT];

size_t succs_sizes[MAX_WORD_COUNT];


void replace_non_printable_chars_with_space()
{
    for (size_t i = 0; book[i] != '\0'; i++)
    {
        unsigned char c = book[i];
        if (!isprint(c) && c != '\n') //replace non-printable characters. Not lewline.
        {
            book[i] = ' ';
        }
    }
}

/// Returns token id if token is known, otherwise creates a new entry and
/// returns the new token id
size_t token_id(char *token)
{
	for (size_t i = 0; i < tokens_size; i++)
		{
			if (strcmp(tokens[i], token) == 0)
				return i; 
			else ;
		}

    if (tokens_size >= MAX_WORD_COUNT) //if overflow
    {
        printf("Token too big");
        exit(1);
    }

    tokens[tokens_size] = strdup(token);  // store a copy
    return tokens_size++;

}

/// Appends a token to the successors list of a `token`
void append_to_succs(char *token, char *succ) {
	size_t id = token_id(token);
    size_t succ_id = token_id(succ);

    if (succs_sizes[id] >= MAX_SUCCESSOR_COUNT){ //too many successors
        return; //return nothing to not overflow the count
    }

    // Prevent duplicates
    for (size_t i = 0; i < succs_sizes[id]; i++)
	{
        if (strcmp(succs[id][i], tokens[succ_id]) == 0)
            return; //already exists
    }
    succs[id][succs_sizes[id]++] = tokens[succ_id]; //is new
}

/// Creates tokens from the `book` and fills `tokens` and `succs` arrays using
/// the `token`s.
void tokenize_and_fill_succs(char *delimiters, char *str)
{
	
	char *prev_token = NULL;
    char *token = strtok(str, delimiters); //delimiters are . ! ?
	while (token)
	{
		size_t id = token_id(token); //adds token to tokens
		if (prev_token != NULL)
        {
            append_to_succs(prev_token, token);
        }

        prev_token = token;  // current token becomes previous
        token = strtok(NULL, delimiters);
    }
}

/// Returns last character of a string
char last_char(char *str)
{
      if (!str || strlen(str) == 0)
        return '\0';
    return str[strlen(str) - 1];

}

/// Returns whether the token ends with `!`, `?` or `.`.
bool token_ends_a_sentence(char *token) //returns true if token ends a sentence
{
    if (token == NULL || *token == '\0')
        return false;

    char last = token[strlen(token) - 1];
    return (last == '.' || last == '!' || last == '?');
}


/// Returns a random `token_id` that corresponds to a `token` that starts with a
/// capital letter.
/// Uses `tokens`, `tokens_size.
size_t random_token_id_that_starts_a_sentence() {
	size_t candidates[MAX_WORD_COUNT];
    size_t count = 0;

    for (size_t i = 0; i < tokens_size; i++)
	{
        if (isupper((unsigned char)tokens[i][0]))
		{
            candidates[count++] = i;
        }
    }

    if (count == 0) // fallback
        return rand() % tokens_size;

    return candidates[rand() % count];
}

/// Generates a random sentence using `tokens`, `succs`, and `succs_sizes`.
/// The sentence array will be filled up to `sentence_size-1` characters using
/// random tokens until:
/// - a token is found where `token_ends_a_sentence()`
/// - or more tokens cannot be concatenated to the `sentence` anymore.
///  Returns the filled sentence array.
char *generate_sentence(char *sentence, size_t sentence_size) {
    sentence[0] = '\0'; // reset sentence

    size_t current_token_id = random_token_id_that_starts_a_sentence();
    char *token = tokens[current_token_id];

    strncat(sentence, token, sentence_size - strlen(sentence) - 2);

    while (!token_ends_a_sentence(token))
	{
        if (succs_sizes[current_token_id] == 0)
            break;

        size_t next_id = rand() % succs_sizes[current_token_id];
        token = succs[current_token_id][next_id];

        if (strlen(sentence) + strlen(token) + 2 >= sentence_size)
            break;

        strcat(sentence, " ");
        strcat(sentence, token);

        current_token_id = token_id(token);
    }
    return sentence;
}

int main() {
  replace_non_printable_chars_with_space();

  char *delimiters = " ,!?\n\r";
  tokenize_and_fill_succs(delimiters, book);

  char sentence[1000];
  srand(time(NULL));

  // Initialize `sentence` and then find a question sentence.
  do {
    // YOUR CODE HERE
    generate_sentence(sentence, sizeof sentence);
  } while (last_char(sentence) != '?');
  puts(sentence);
  puts("");

  // Initialize `sentence` and then find a sentence ending with exclamation
  // mark.
  do {
    // YOUR CODE HERE
    generate_sentence(sentence, sizeof sentence);
  } while (last_char(sentence) != '!');
  puts(sentence);
}