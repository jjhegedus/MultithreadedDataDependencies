#pragma once

#include "pch.h"

#include <map>
#include <vector>
#include <tuple>

namespace ndtech {

  template <typename ItemType>
  struct NamedItemStore {

    std::map<std::string, ItemType> m_items;
    std::mutex m_itemsMutex;
    std::condition_variable m_itemsCV;

    std::vector<std::tuple<std::string, ItemType, std::string>> m_itemActions;
    std::mutex m_itemActionsMutex;

    bool m_running = true;

    

    void RunOnThread(std::thread& thread) {
      thread = std::thread(&NamedItemStore::Run, this);
    }

    void Run() {
      while (m_running) {
        {
          std::lock_guard lock(m_itemActionsMutex);
          for (auto itemAction : m_itemActions) {
            if (std::get<2>(itemAction) == "a") {
              std::lock_guard itemsLock(m_itemsMutex);
              m_items.insert_or_assign(std::get<0>(itemAction), std::get<1>(itemAction));
              m_itemsCV.notify_all();
              std::cout << "NamedItemStore threadId = " << std::this_thread::get_id() << ": added item itemName = " << std::get<0>(itemAction) << " itemValue = " << std::get<1>(itemAction) << std::endl;
            }
            else if (std::get<2>(itemAction) == "r") {
              std::lock_guard itemsLock(m_itemsMutex);
              auto foundItem = m_items.find(std::get<0>(itemAction));
              if (foundItem != m_items.end()) {
                m_items.erase(foundItem);
                m_itemsCV.notify_all();
                std::cout << "NamedItemStore threadId = " << std::this_thread::get_id() << ": removed item itemName = " << std::get<0>(itemAction) << std::endl;
              }
              else {
                std::cout << "NamedItemStore threadId = " << std::this_thread::get_id() << ": unable to remove item itemName = " << std::get<0>(itemAction) << " because it does not exist." << std::endl;
              }
            }
          }

          m_itemActions.clear();

        }
      }
    }

    void Stop() {
      m_running = false;
    }

    void AddItem(std::string key, ItemType value) {
      std::lock_guard lock(m_itemActionsMutex);
      m_itemActions.push_back(std::make_tuple(key, value, "a"));
    }

    void RemoveItem(std::string itemName) {
      std::lock_guard lock(m_itemActionsMutex);
      m_itemActions.push_back(std::make_tuple(itemName, "", "r"));
    }

    auto GetItem(std::string itemName) {
      std::unique_lock lock(m_itemsMutex);
      while (m_items.find(itemName) == m_items.end()) {
        m_itemsCV.wait(lock);
      }
      return m_items.find(itemName);
    }

  };

}