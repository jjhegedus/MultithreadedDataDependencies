// MultithreadedDataDependencies.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <tuple>
#include "ThreadedConsoleInput.h"
#include "NamedItemStore.h"
#include "ThreadSafeQueue.h"
#include "ThreadedQueueProcessor.h"

template <typename Range, typename Callable>
void transform_print(const Range& r, Callable c) {
  for (const auto& e : r) {
    std::cout << std::invoke(c, e) << std::endl;
  }
}

int main()
{
  std::cout << "main threadId = " << std::this_thread::get_id() << "\n\n";
  std::cout << "Test data dependencies by using the following commands\n\n" <<
    "  p<enter> - 'print' - Launches the print command and requests the item name you wish to print.\n" <<
    "  Type in an item name without spaces and hit enter.  Any name will do since nothing exists yet.\n"
    "  The print command will wait until the requested data exists, then print it.\n" <<
    "  However, this program uses multiple threads to allow you to continue to enter additional commands while you wait.\n\n" <<
    "  a<enter> - 'add item' - Launches the add item command and asks for the name of the item to add.\n" <<
    "  Type in the item name you previously passed to the print command.\n" <<
    "  Next, the add item command will ask for the value of the item to add.\n" <<
    "  Type in the value of the item and hit enter.\n" <<
    "  The add item command will indicate that the item has been added.\n" <<
    "  The print command will now complete because it is notified that the item it was looking for now exists.\n\n" <<
    "  r<enter> - 'remove' - This will launch the remove command and asks for the name of the item to remove\n" <<
    "  Type in the name of an item which will be removed if it exists.\n\n"
    "  q<enter> - 'quit' - This will end the program.\n\n";

  ndtech::ThreadSafeQueue<std::string> tsq;

  ndtech::ThreadedConsoleInput threadedConsoleInput(&tsq);
  std::thread inputThread;
  threadedConsoleInput.runOnThread(inputThread);

  std::thread queueProcessingThread;
  ndtech::ThreadedQueueProcessor<std::string> qp(&tsq);
  qp.runOnThread(queueProcessingThread);

  inputThread.join();
  queueProcessingThread.join();

  //std::vector<std::pair<int, int>> v{ {4,40}, {5,50}, {6,60} };
  //transform_print(v, [](const auto& p) {return p.first * p.second; });
  //transform_print(v, &std::pair<int, int>::second);

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
