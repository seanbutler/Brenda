#include "ECS/ECS.h"
#include <iostream>

/**
 * @brief Simple test demonstrating the new ECS framework.
 * 
 * This can be called from main() to verify the ECS is working correctly.
 */
void testNewECSFramework() {
    std::cout << "\n=== Testing New ECS Framework ===" << std::endl;
    
    // Create ECS world
    ECS::World world;
    
    // Add movement system
    auto* movementSystem = world.addSystem<ECS::MovementSystem>(&world);
    std::cout << "Added system: " << movementSystem->getName() << std::endl;
    
    // Create test entities
    ECS::EntityID npc1 = world.createEntity();
    ECS::EntityID npc2 = world.createEntity();
    ECS::EntityID npc3 = world.createEntity();
    
    std::cout << "Created entities: " << npc1 << ", " << npc2 << ", " << npc3 << std::endl;
    
    // Add components to entities
    world.addComponent<PositionComponent>(npc1, 0.0f, 0.0f);
    world.addComponent<DescriptionComponent>(npc1, "Guard", "A vigilant guard");
    auto* movement1 = world.addComponent<Movement>(npc1, 100.0f);
    movement1->setTarget(100.0f, 50.0f);
    
    world.addComponent<PositionComponent>(npc2, 50.0f, 50.0f);
    world.addComponent<DescriptionComponent>(npc2, "Merchant", "A friendly trader");
    world.addComponent<InfoBoxComponent>(npc2, "Merchant\nSelling goods");
    
    world.addComponent<PositionComponent>(npc3, 25.0f, 25.0f);
    auto* movement3 = world.addComponent<Movement>(npc3, 75.0f);
    movement3->setTarget(200.0f, 150.0f);
    
    std::cout << "Added components to entities" << std::endl;
    
    // Test component access
    auto* pos1 = world.getComponent<PositionComponent>(npc1);
    auto* desc1 = world.getComponent<DescriptionComponent>(npc1);
    
    if (pos1 && desc1) {
        std::cout << desc1->name << " is at position (" << pos1->x << ", " << pos1->y << ")" << std::endl;
    }
    
    // Test component queries
    std::cout << "NPC1 has Movement: " << (world.hasComponent<Movement>(npc1) ? "Yes" : "No") << std::endl;
    std::cout << "NPC2 has Movement: " << (world.hasComponent<Movement>(npc2) ? "Yes" : "No") << std::endl;
    std::cout << "NPC2 has InfoBox: " << (world.hasComponent<InfoBoxComponent>(npc2) ? "Yes" : "No") << std::endl;
    
    // Simulate a few update cycles
    std::cout << "\nSimulating movement updates..." << std::endl;
    for (int i = 0; i < 3; ++i) {
        world.update(0.1f); // 100ms updates
        
        auto* currentPos = world.getComponent<PositionComponent>(npc1);
        if (currentPos) {
            std::cout << "Frame " << i+1 << ": Guard at (" << currentPos->x << ", " << currentPos->y << ")" << std::endl;
        }
    }
    
    // Test entity destruction
    std::cout << "\nTesting entity destruction..." << std::endl;
    std::cout << "Entities before destruction: " << world.getEntityCount() << std::endl;
    
    bool destroyed = world.destroyEntity(npc2);
    std::cout << "Destroyed NPC2: " << (destroyed ? "Success" : "Failed") << std::endl;
    std::cout << "Entities after destruction: " << world.getEntityCount() << std::endl;
    
    // Verify entity is gone
    auto* destroyedPos = world.getComponent<PositionComponent>(npc2);
    std::cout << "NPC2 position after destruction: " << (destroyedPos ? "Still exists!" : "Properly removed") << std::endl;
    
    std::cout << "\n=== ECS Framework Test Complete ===" << std::endl;
}
