import re
import sys


def text_to_words(input_file, output_file):

    with open(input_file, "r", encoding="utf-8") as f:
        text = f.read()

    # Lowercase
    text = text.lower()

    # Replace non-letters with space
    text = re.sub(r"[^a-z0-9]+", " ", text)

    # Split into words
    words = text.split()

    # Write one per line
    with open(output_file, "w", encoding="utf-8") as f:
        for w in words:
            f.write(w + "\n")

    print(f"[+] Wrote {len(words)} words to {output_file}")


if __name__ == "__main__":

    if len(sys.argv) != 3:
        print("Usage: python text_to_words.py input.txt output.txt")
        sys.exit(1)

    text_to_words(sys.argv[1], sys.argv[2])

