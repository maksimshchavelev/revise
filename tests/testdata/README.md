## Overview

The `testdata` directory contains test fixtures used to verify the functionality of the IO module.

## Test Fixtures

- **`deck.rpkg`** — A sample deck file in Revise format containing:
    - **Metadata:** Name `"deck"`, description `"description"`.
    - **Limits:** 1s time limit, 2 new cards, 3 reviews, and 4 incorrect cards.
    - **Cards (no images):**
        - Front: `"<p>Card 1</p>"` | Back: `"<p>Back of card 1</p>"`
        - Front: `"<p>Card 2</p>"` | Back: `"<p>Back of card 2</p>"`

- **`broken.rpkg`** — A broken file with random binary data inside