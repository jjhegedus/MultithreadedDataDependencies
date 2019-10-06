#pragma once

#include "ThreadSafeQueue.h"
#include "ThreadPool.h"

namespace ndtech {

  template <typename ItemType>
  struct ThreadedQueueProcessor {

    ndtech::ThreadSafeQueue<std::string>* m_q;
    bool m_running = true;
    NamedItemStore<std::string> m_store;

    ndtech::ThreadPool tp = ThreadPool(2);

    ThreadedQueueProcessor(ndtech::ThreadSafeQueue<std::string>* q) {
      m_q = q;
    }

    void runOnThread(std::thread& thread) {
      thread = std::thread(&ThreadedQueueProcessor::Run, this);
    }


    void Run() {
      std::thread itemStoreThread;
      m_store.RunOnThread(itemStoreThread);

      while (m_running) {
        ItemType item = m_q->Pop();

        if (item == "q") {
          std::cout << "Received quit signal" << std::endl;
          m_running = false;
          break;
        }
        if (item == "a") {

          std::thread t([this]() {

            std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Adding Item:  Enter Item Name: ";
            std::string newName = m_q->Pop();
            std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Adding Item:  Enter Item value: ";
            std::string newValue = m_q->Pop();
            std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Adding Item: NAME = " << newName << " VALUE = " << newValue << std::endl;

            tp.doJob([this, newName, newValue]() {m_store.AddItem(newName, newValue); });
            //m_store.AddItem(newName, newValue);
            });

          t.join();
        }
        else if (item == "p") {
          std::thread t([this]() {

            std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Printing Item:  Enter Item Name: ";
            std::string itemName = m_q->Pop();
            std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Printing Item: NAME = " << itemName << std::endl;

            tp.doJob([this, itemName]() {
              auto returnedItem = m_store.GetItem(itemName);
              std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Got Item: NAME = " << itemName << " VALUE = " << (*returnedItem).second << std::endl;
              });
             

            });

          t.join();
        }
        else if (item == "r") {
          std::thread t([this]() {

            std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Removing Item:  Enter Item Name: ";
            std::string itemName = m_q->Pop();
            std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": Removing Item: NAME = " << itemName << std::endl;

            tp.doJob([this, itemName]() {
              m_store.RemoveItem(itemName);
              std::cout << "ThreadedQueueProcessor threadId = " << std::this_thread::get_id() << ": ThreadedQueueProcessor:  Removed Item: NAME = " << itemName << std::endl;
              });

            });

          t.join();
        }

        else {
          std::cout << "Read '" << item << "' from stdin, which is not a valid message" << std::endl;
        }
      }

      m_store.Stop();
      itemStoreThread.join();
    }

  };

}