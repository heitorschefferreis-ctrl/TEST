#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <algorithm>
#include <vector>
#include <cmath>

using namespace geode::prelude;

// Structures to keep track of scanned objects for dynamic lookahead
struct PathfinderHazard {
    cocos2d::CCPoint position;
    float width;
    float height;
    bool isSpike;
};

struct PathfinderOrb {
    GameObject* object;
    cocos2d::CCPoint position;
    bool isDash;
    float radius;
};

// We use Geode's field system to assign safe custom properties to PlayLayer cleanly.
$modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        cocos2d::CCDrawNode* m_radarNode = nullptr;
        bool m_wasHolding = false;
        float m_holdTimer = 0.0f;
        bool m_isDashing = false;
        GameObject* m_currentDashOrb = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontSave) {
        if (!PlayLayer::init(level, useReplay, dontSave)) return false;

        // Create visual debugger overlay if configured
        if (Geode::get()->getSettingValue<bool>("draw-radar")) {
            m_fields->m_radarNode = cocos2d::CCDrawNode::create();
            this->addChild(m_fields->m_radarNode, 99999);
        }

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        // Check if pathfinder is enabled
        if (!Geode::get()->getSettingValue<bool>("autopilot-enabled")) {
            if (m_fields->m_radarNode) m_fields->m_radarNode->clear();
            return;
        }

        auto player = this->m_player1;
        if (!player || player->m_isDead) return;

        // Get settings parameters
        float lookahead = static_cast<float>(Geode::get()->getSettingValue<double>("lookahead-distance"));

        // Reset drawing radar every frame
        if (m_fields->m_radarNode) {
            m_fields->m_radarNode->clear();
        }

        // Scan environment for obstacles and triggers
        std::vector<PathfinderHazard> hazards;
        std::vector<PathfinderOrb> orbs;
        
        float px = player->getPositionX();
        float py = player->getPositionY();

        // Iterate over dynamic physical structures currently loaded near the player
        auto objects = this->m_objects;
        if (objects) {
            CCObject* objPtr;
            CCARRAY_FOREACH(objects, objPtr) {
                auto obj = castTo<GameObject*>(objPtr);
                if (!obj) continue;

                float ox = obj->getPositionX();
                float oy = obj->getPositionY();

                // Only inspect items ahead within lookahead range
                if (ox < px - 30.0f || ox > px + lookahead) continue;

                // Categorize object
                if (obj->m_isHazard) {
                    hazards.push_back({ {ox, oy}, obj->getScaledContentSize().width, obj->getScaledContentSize().height, true });
                    
                    // Draw visual radar indicator around active hazards
                    if (m_fields->m_radarNode) {
                        m_fields->m_radarNode->drawCircle(
                            ccp(ox, oy), 
                            15.0f, 
                            cocos2d::ccc4f(1.0f, 0.0f, 0.0f, 0.5f), 
                            1.0f, 
                            cocos2d::ccc4f(1.0f, 0.0f, 0.0f, 0.8f),
                            10
                        );
                    }
                } else if (obj->m_objectType == GameObjectType::Orb || obj->m_objectType == GameObjectType::PinkOrb || 
                           obj->m_objectType == GameObjectType::RedOrb || obj->m_objectType == GameObjectType::DashOrb) {
                    
                    bool isDash = (obj->m_objectType == GameObjectType::DashOrb);
                    orbs.push_back({ obj, {ox, oy}, isDash, 50.0f });

                    if (m_fields->m_radarNode) {
                        m_fields->m_radarNode->drawCircle(
                            ccp(ox, oy), 
                            30.0f, 
                            cocos2d::ccc4f(0.0f, 1.0f, 0.0f, 0.4f), 
                            1.0f, 
                            cocos2d::ccc4f(0.0f, 1.0f, 0.0f, 0.6f),
                            12
                        );
                    }
                }
            }
        }

        // Render radar visual projection cone
        if (m_fields->m_radarNode) {
            m_fields->m_radarNode->drawSegment(
                ccp(px, py), 
                ccp(px + lookahead, py), 
                1.0f, 
                cocos2d::ccc4f(0.0f, 0.8f, 1.0f, 0.3f)
            );
        }

        // Decision engine
        bool shouldHold = false;
        bool processedAction = false;

        // 1. Interactive Ring/Orb trigger logic
        for (const auto& orb : orbs) {
            float dist = ccpDistance(player->getPosition(), orb.position);
            if (dist < orb.radius) {
                if (orb.isDash) {
                    m_fields->m_isDashing = true;
                    m_fields->m_currentDashOrb = orb.object;
                    shouldHold = true;
                    processedAction = true;
                    break;
                } else {
                    // Perform instant click trigger
                    this->pushButton(0, true);
                    this->releaseButton(0, true);
                    processedAction = true;
                    break;
                }
            }
        }

        // Manage active dash state
        if (m_fields->m_isDashing && m_fields->m_currentDashOrb) {
            // End dash once we pass the dash target
            if (px > m_fields->m_currentDashOrb->getPositionX() + 80.0f) {
                m_fields->m_isDashing = false;
                m_fields->m_currentDashOrb = nullptr;
            } else {
                shouldHold = true;
                processedAction = true;
            }
        }

        if (processedAction) {
            executeHoldState(shouldHold);
            return;
        }

        // 2. Fly modes optimization (Ship, Wave, Swing, Bird/UFO)
        if (player->m_isShip || player->m_isDart || player->m_isBird || player->m_isSwing) {
            float safeUpperY = py + 100.0f;
            float safeLowerY = py - 100.0f;

            // Find constraints from hazards ahead
            for (const auto& haz : hazards) {
                if (haz.position.x > px && haz.position.x < px + 120.0f) {
                    if (haz.position.y > py) {
                        safeUpperY = std::min(safeUpperY, haz.position.y - (haz.height / 2.0f) - 15.0f);
                    } else {
                        safeLowerY = std::max(safeLowerY, haz.position.y + (haz.height / 2.0f) + 15.0f);
                    }
                }
            }

            float midTargetY = (safeUpperY + safeLowerY) / 2.0f;

            // Visual target marker
            if (m_fields->m_radarNode) {
                m_fields->m_radarNode->drawSegment(
                    ccp(px + 30.0f, midTargetY), 
                    ccp(px + 70.0f, midTargetY), 
                    2.0f, 
                    cocos2d::ccc4f(1.0f, 0.8f, 0.0f, 0.9f)
                );
            }

            bool normalGravity = !player->m_isUpsideDown;
            if (player->m_isDart) { // Wave dynamics: fast toggle actions
                if (normalGravity) {
                    shouldHold = (py < midTargetY);
                } else {
                    shouldHold = (py > midTargetY);
                }
            } else if (player->m_isBird) { // UFO mechanics: pulsing jumps
                if (normalGravity && py < midTargetY - 10.0f) {
                    this->pushButton(0, true);
                    this->releaseButton(0, true);
                } else if (!normalGravity && py > midTargetY + 10.0f) {
                    this->pushButton(0, true);
                    this->releaseButton(0, true);
                }
            } else { // Ship and Swing
                if (normalGravity) {
                    shouldHold = (py < midTargetY);
                } else {
                    shouldHold = (py > midTargetY);
                }
            }
        }
        // 3. Platformer Ground Modes (Cube, Ball, Robot, Spider)
        else {
            bool hazardAhead = false;
            float hazardDistance = 9999.0f;
            for (const auto& haz : hazards) {
                float distanceX = haz.position.x - px;
                if (distanceX > 0.0f && distanceX < 85.0f) {
                    // Verify hazard is on the player's active gravity level
                    bool normalGravity = !player->m_isUpsideDown;
                    bool alignedY = normalGravity ? (haz.position.y < py + 20.0f) : (haz.position.y > py - 20.0f);
                    
                    if (alignedY) {
                        hazardAhead = true;
                        if (distanceX < hazardDistance) {
                            hazardDistance = distanceX;
                        }
                    }
                }
            }

            if (hazardAhead) {
                if (player->m_isBall || player->m_isSpider) {
                    // Immediate gravity switch
                    this->pushButton(0, true);
                    this->releaseButton(0, true);
                } else {
                    // Cube or Robot Jump execution
                    if (player->m_isGrounded) {
                        shouldHold = true;
                        m_fields->m_holdTimer = 0.22f; // Keep jump pressed briefly to clear wide gaps
                    }
                }
            }

            // Manage jump hold timers
            if (m_fields->m_holdTimer > 0.0f) {
                shouldHold = true;
                m_fields->m_holdTimer -= dt;
            }
        }

        executeHoldState(shouldHold);
    }

    // Helper function to safely orchestrate key holds
    void executeHoldState(bool state) {
        if (state && !m_fields->m_wasHolding) {
            this->pushButton(0, true);
            m_fields->m_wasHolding = true;
        } else if (!state && m_fields->m_wasHolding) {
            this->releaseButton(0, true);
            m_fields->m_wasHolding = false;
        }
    }
};

// Pause UI Integration to adjust autopilot status during action pauses
$modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto menu = this->getChildByID("right-button-menu");
        if (!menu) {
            menu = this->getChildByID("left-button-menu");
        }

        if (menu) {
            auto sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
            sprite->setScale(0.4f);
            auto btn = CCMenuItemSpriteExtra::create(
                sprite,
                this,
                menu_selector(MyPauseLayer::onToggleAutopilot)
            );
            btn->setID("autopilot-toggle");
            menu->addChild(btn);
            menu->updateLayout();
        }
    }

    void onToggleAutopilot(cocos2d::CCObject* sender) {
        bool state = Geode::get()->getSettingValue<bool>("autopilot-enabled");
        Geode::get()->setSettingValue<bool>("autopilot-enabled", !state);
        
        auto status = !state ? "Autopilot Enabled!" : "Autopilot Disabled!";
        FLAlertLayer::create("OmniPathfinder AI", status, "OK")->show();
    }
};