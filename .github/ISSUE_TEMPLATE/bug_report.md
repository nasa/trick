---
name: Bug report
about: Create a report to help us improve
title: "[Short and descriptive title goes here]"
labels: bug
assignees: ''

---

**Describe the bug**
A clear and concise description of what the bug is.

**Minimal working example**
This may be tough since Trick sims can have many moving parts and you definitely shouldn't share your employer's code. Try to start over with the function or file that the issue occurs in and include it in a code block like the example below:
```C++
#include <iostream>
void buggy_function() {
  double a = 1.0;
  double b = 2.1;
  double c = a + b;
  std::cout << "a + b = " << c << std::endl; 
  // This should output 3.1, but for some reason outputs 3! (not really, just an example)
}
```

**Expected behavior**
A clear and concise description of what you expected to happen.

**Screenshots**
If applicable, add screenshots to help explain your problem.

**Desktop environment:**
 - OS: [e.g. Ubuntu]
 - Version [e.g. 22.04 LTS]

**Additional context**
Add any other context about the problem here.
