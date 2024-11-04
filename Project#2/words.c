#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // For read(), write(), close()
#include <fcntl.h>      // For open()
#include <string.h>     // For strcmp(), strdup()
#include <ctype.h>      // For isalpha()
#include <sys/stat.h>   // For stat()

#define BUFFER_SIZE 4096
#define HASH_TABLE_SIZE 10007  // A prime number for better distribution

/*
 * Structure: WordEntry
 * --------------------
 * Represents an entry in the hash table.
 *
 * Fields:
 *   word  - The word string.
 *   count - The number of occurrences of the word.
 *   next  - Pointer to the next entry in the linked list (for collision handling).
 */
typedef struct WordEntry {
    char *word;
    int count;
    struct WordEntry *next;
} WordEntry;

/* Global hash table */
WordEntry *hash_table[HASH_TABLE_SIZE] = {NULL};

/*
 * Function: hash_function
 * -----------------------
 * Computes the hash value of a given string.
 *
 * Parameters:
 *   str - The input string.
 *
 * Returns:
 *   The hash value as an unsigned integer.
 */
unsigned int hash_function(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    return hash % HASH_TABLE_SIZE;
}

/*
 * Function: is_word_char
 * ----------------------
 * Determines if a character is valid within a word based on specified rules.
 *
 * Parameters:
 *   c - The character to check.
 *
 * Returns:
 *   1 if the character is a letter, apostrophe, or hyphen; 0 otherwise.
 */
int is_word_char(char c) {
    return isalpha(c) || c == '\'' || c == '-';
}

/*
 * Function: insert_word
 * ---------------------
 * Inserts a word into the hash table or increments its count if it already exists.
 *
 * Parameters:
 *   word - The word to insert.
 *
 * Returns:
 *   void
 */
void insert_word(const char *word) {
    unsigned int hash = hash_function(word);
    WordEntry *entry = hash_table[hash];

    while (entry != NULL) {
        if (strcmp(entry->word, word) == 0) {
            entry->count++;
            return;
        }
        entry = entry->next;
    }

    // Word not found; create a new entry
    WordEntry *new_entry = (WordEntry *)malloc(sizeof(WordEntry));
    new_entry->word = strdup(word);
    new_entry->count = 1;
    new_entry->next = hash_table[hash];
    hash_table[hash] = new_entry;
}

/*
 * Function: process_file
 * ----------------------
 * Processes a given file to count the occurrences of each word.
 *
 * Steps:
 * 1. Open the file using open().
 * 2. Read the file content using read(), in chunks.
 * 3. Tokenize the input into words according to the specified rules.
 *    - Words are sequences of letters, apostrophes, and hyphens.
 *    - Hyphens can appear in a word only if preceded and followed by a letter.
 * 4. For each word found:
 *    a. Convert the word into a null-terminated string.
 *    b. Insert or update the word count in the hash table.
 * 5. Close the file using close().
 *
 * Parameters:
 *   filename - The name of the file to process.
 *
 * Returns:
 *   0 on success, -1 on failure.
 */
int process_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    char word_buffer[256];
    int word_len = 0;
    char prev_char = '\0';

    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            char c = buffer[i];

            if (is_word_char(c)) {
                // Handle hyphen rules
                if (c == '-') {
                    if (!isalpha(prev_char) || (i + 1 < bytes_read && !isalpha(buffer[i + 1]))) {
                        continue;  // Skip invalid hyphens
                    }
                }
                if (word_len < sizeof(word_buffer) - 1) {
                    word_buffer[word_len++] = c;
                }
            } else {
                if (word_len > 0) {
                    word_buffer[word_len] = '\0';
                    insert_word(word_buffer);
                    word_len = 0;
                }
            }
            prev_char = c;
        }
    }

    // Handle the last word in the buffer
    if (word_len > 0) {
        word_buffer[word_len] = '\0';
        insert_word(word_buffer);
    }

    if (bytes_read < 0) {
        perror("read");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

/*
 * Function: collect_words
 * -----------------------
 * Collects all words from the hash table into an array.
 *
 * Parameters:
 *   total_words - Pointer to an integer to store the total number of words collected.
 *
 * Returns:
 *   An array of pointers to WordEntry.
 */
WordEntry **collect_words(int *total_words) {
    int capacity = 1024;
    int count = 0;
    WordEntry **array = (WordEntry **)malloc(sizeof(WordEntry *) * capacity);

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        WordEntry *entry = hash_table[i];
        while (entry != NULL) {
            if (count >= capacity) {
                capacity *= 2;
                array = (WordEntry **)realloc(array, sizeof(WordEntry *) * capacity);
            }
            array[count++] = entry;
            entry = entry->next;
        }
    }

    *total_words = count;
    return array;
}

/*
 * Function: compare_words
 * -----------------------
 * Comparison function for sorting words.
 *
 * Parameters:
 *   a - Pointer to a pointer to WordEntry.
 *   b - Pointer to a pointer to WordEntry.
 *
 * Returns:
 *   Negative if a < b, zero if a == b, positive if a > b.
 */
int compare_words(const void *a, const void *b) {
    WordEntry *entryA = *(WordEntry **)a;
    WordEntry *entryB = *(WordEntry **)b;

    if (entryB->count != entryA->count) {
        return entryB->count - entryA->count;  // Decreasing count
    } else {
        return strcmp(entryA->word, entryB->word);  // Lexicographical order
    }
}

/*
 * Function: output_results
 * ------------------------
 * Outputs the list of words and their counts sorted by decreasing count.
 * Words with the same count are sorted lexicographically.
 *
 * Steps:
 * 1. Collect all the words and counts from the hash table into an array.
 * 2. Sort the array first by decreasing count, then lexicographically.
 * 3. For each word in the sorted array:
 *    a. Print the word and its count using write().
 *
 * Returns:
 *   void
 */
void output_results() {
    int total_words = 0;
    WordEntry **words_array = collect_words(&total_words);

    qsort(words_array, total_words, sizeof(WordEntry *), compare_words);

    char output_buffer[512];
    for (int i = 0; i < total_words; i++) {
        WordEntry *entry = words_array[i];
        int len = snprintf(output_buffer, sizeof(output_buffer), "%s %d\n", entry->word, entry->count);
        write(STDOUT_FILENO, output_buffer, len);
    }

    free(words_array);
}
