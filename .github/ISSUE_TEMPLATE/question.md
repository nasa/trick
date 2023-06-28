---
name: Question
about: Ask for help
title: "[Short and descriptive title goes here]"
labels: question
assignees: ''

---

**Research**
Before posting a question, spend a reasonable amount of time researching the problem (is there a name for what you're trying to do?) and searching the internet for answers (has someone already found the answer in another issue in this repository or on Stack Overflow?). Keep track of what you find, even if it doesn't help! Provide relevant links, along with why they don't answer your question.

**Introduce the problem**
Explain how you encountered the problem, and any difficulties that have prevented you from solving it yourself.

**Minimal working example**
This may be tough since Trick sims can have many moving parts and you definitely shouldn't share your employer's code. Try to start over with the function or file that the issue occurs in and include it in a code block like the example below:
```C++
#include <iostream>
void function_i_need_help_with() {
  int a = 1;
  double b = 2.1;
  int c = a + b;
  std::cout << "a + b = " << c << std::endl;  // Why doesn't it output 3.1?
}
```
