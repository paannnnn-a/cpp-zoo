### 工作前请先阅读docs目录下的接口文档

### 以下是仓库结构
```
cpp-zoo/
│
├── README.md                              [共同]
│
├── main.cpp                              [共同]
│
├── CMakeLists.txt / Makefile             [共同，可选]
│
│
├── include/
│   │
│   │  ======== 公共接口 / 公共工具 ========
│   │
│   ├── AnimalCommon.h                    [A]
│   │   └── AnimalStatus
│   │       AnimalBasicInfo
│   │       ZoneInfo
│   │       animalStatusToString()
│   │
│   ├── InputValidator.h                  [共同]
│   ├── Logger.h                          [共同，可选]
│   └── UserCommon.h                      [共同/C，视最终登录设计]
│
│   │
│   │  ======== A：动物与健康管理 ========
│   │
│   ├── Animal.h                          [A]
│   ├── Mammal.h                          [A]
│   ├── Bird.h                            [A]
│   ├── Reptile.h                         [A]
│   │
│   ├── AnimalManager.h                   [A]
│   ├── AnimalMenu.h                      [A]
│   │
│   ├── Zone.h                            [A]
│   ├── ZoneManager.h                     [A]
│   │
│   ├── MedicalRecord.h                   [A]
│   ├── HealthCheckRecord.h               [A]
│   ├── DiseaseRecord.h                   [A]
│   ├── TreatmentRecord.h                 [A]
│   ├── RecoveryRecord.h                  [A]
│   ├── HealthManager.h                   [A]
│   └── HealthMenu.h                      [A]
│
│   │
│   │  ======== B：饲养与物资管理 ========
│   │
│   ├── Food.h                            [B]
│   ├── PlantFood.h                       [B]
│   ├── MeatFood.h                        [B]
│   ├── FruitFood.h                       [B]
│   │
│   ├── InventoryManager.h                [B]
│   ├── InventoryMenu.h                   [B]
│   │
│   ├── FeedingPlan.h                     [B]
│   ├── FeedingRecord.h                   [B]
│   ├── FeedingManager.h                  [B]
│   └── FeedingMenu.h                     [B]
│
│   │
│   │  ======== C：游客与票务服务 ========
│   │
│   ├── Visitor.h                         [C]
│   ├── VisitorManager.h                  [C]
│   ├── VisitorMenu.h                     [C]
│   │
│   ├── Ticket.h                          [C]
│   ├── AdultTicket.h                     [C]
│   ├── StudentTicket.h                   [C]
│   ├── ChildTicket.h                     [C]
│   │
│   ├── Order.h                           [C]
│   ├── TicketManager.h                   [C]
│   ├── TicketMenu.h                      [C]
│   │
│   ├── Feedback.h                        [C]
│   └── FeedbackManager.h                 [C]
│
│
├── src/
│   │
│   │  ======== 公共实现 ========
│   │
│   ├── InputValidator.cpp                [共同]
│   ├── Logger.cpp                        [共同，可选]
│   │
│   │  ======== A：动物与健康管理 ========
│   │
│   ├── AnimalCommon.cpp                  [A]
│   ├── Animal.cpp                        [A]
│   ├── Mammal.cpp                        [A]
│   ├── Bird.cpp                          [A]
│   ├── Reptile.cpp                       [A]
│   │
│   ├── AnimalManager.cpp                 [A]
│   ├── AnimalMenu.cpp                    [A]
│   │
│   ├── Zone.cpp                          [A]
│   ├── ZoneManager.cpp                   [A]
│   │
│   ├── MedicalRecord.cpp                 [A]
│   ├── HealthCheckRecord.cpp             [A]
│   ├── DiseaseRecord.cpp                 [A]
│   ├── TreatmentRecord.cpp               [A]
│   ├── RecoveryRecord.cpp                [A]
│   ├── HealthManager.cpp                 [A]
│   └── HealthMenu.cpp                    [A]
│
│   │
│   │  ======== B：饲养与物资管理 ========
│   │
│   ├── Food.cpp                          [B]
│   ├── PlantFood.cpp                     [B]
│   ├── MeatFood.cpp                      [B]
│   ├── FruitFood.cpp                     [B]
│   │
│   ├── InventoryManager.cpp              [B]
│   ├── InventoryMenu.cpp                 [B]
│   │
│   ├── FeedingPlan.cpp                   [B]
│   ├── FeedingRecord.cpp                 [B]
│   ├── FeedingManager.cpp                [B]
│   └── FeedingMenu.cpp                   [B]
│
│   │
│   │  ======== C：游客与票务服务 ========
│   │
│   ├── Visitor.cpp                       [C]
│   ├── VisitorManager.cpp                [C]
│   ├── VisitorMenu.cpp                   [C]
│   │
│   ├── Ticket.cpp                        [C]
│   ├── AdultTicket.cpp                   [C]
│   ├── StudentTicket.cpp                 [C]
│   ├── ChildTicket.cpp                   [C]
│   │
│   ├── Order.cpp                         [C]
│   ├── TicketManager.cpp                 [C]
│   ├── TicketMenu.cpp                    [C]
│   │
│   ├── Feedback.cpp                      [C]
│   └── FeedbackManager.cpp               [C]
│
│
├── data/
│   │
│   │  ======== A 管理的数据 ========
│   │
│   ├── animals.txt                       [A]
│   ├── zones.txt                         [A]
│   ├── medical_records.txt               [A]
│   │
│   │  ======== B 管理的数据 ========
│   │
│   ├── foods.txt                         [B]
│   ├── feeding_plans.txt                 [B]
│   ├── feeding_records.txt               [B]
│   │
│   │  ======== C 管理的数据 ========
│   │
│   ├── visitors.txt                      [C]
│   ├── orders.txt                        [C]
│   ├── tickets.txt                       [C，可根据设计合并到 orders]
│   ├── feedback.txt                      [C]
│   │
│   │  ======== 公共数据 ========
│   │
│   ├── users.txt                         [共同/C]
│   └── operation_logs.txt                [共同]
│
│
├── tests/                                 [共同，建议保留]
│   │
│   ├── test_animal.cpp                   [A]
│   ├── test_health.cpp                   [A]
│   ├── test_zone.cpp                     [A]
│   │
│   ├── test_inventory.cpp                [B]
│   ├── test_feeding.cpp                  [B]
│   │
│   ├── test_ticket.cpp                   [C]
│   ├── test_visitor.cpp                  [C]
│   │
│   └── test_integration.cpp              [共同]
│
│
└── docs/
    ├── 三人分工.md                        [共同]
    ├── 统一接口文档.md                    [共同]
    ├── A同学工作流程.md                   [A]
    ├── B同学工作流程.md                   [B]
    └── C同学工作流程.md                   [C]
```
