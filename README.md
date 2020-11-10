# Implementation-of-locks-deadlock-detector-and-priority-inheritance-protocol
• Implemented atomic test and set instruction in assembly in XINU x86 architecture required for the implementation of lock functionality.

• Implemented a spinlock and a busy wait-free lock (by putting current processes to sleep) based on the test and set instruction.

• The busy wait-free implementation was based on the park, unpark, and setpark primitives.

• Modified the lock implementation to automatically detect the presence of deadlocks due to circular dependencies. Also, showed how the use of trylocks in Linux can be used to avoid the deadlocks in the first place.

• Created test cases to show the priority inversion problem in Xinu. Implemented the priority inheritance protocol to solve the priority inversion issue.
