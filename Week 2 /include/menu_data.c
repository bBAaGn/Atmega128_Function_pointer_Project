#include "menu.h"
#include <stddef.h>

// Khai báo trước các mục menu
MenuItem m_main1, m_main2, m_main3, m_main4;
MenuItem m_sub1_1, m_sub1_2;

// 1. Cấp cha (Main Menu)
MenuItem m_main1 = {"1. Che do 1", NULL, &m_sub1_1, &m_main2, NULL, NULL};
MenuItem m_main2 = {"2. Che do 2", NULL, NULL, &m_main3, &m_main1, NULL};
MenuItem m_main3 = {"3. Cai dat", NULL, NULL, &m_main4, &m_main2, NULL};
MenuItem m_main4 = {"4. Thong tin", NULL, NULL, NULL, &m_main3, NULL};

// 2. Cấp con của Mode 1 (Sub Menu)
MenuItem m_sub1_1 = {"1.1 Auto", &m_main1, NULL, &m_sub1_2, NULL, NULL};
MenuItem m_sub1_2 = {"1.2 Manual", &m_main1, NULL, NULL, &m_sub1_1, NULL};