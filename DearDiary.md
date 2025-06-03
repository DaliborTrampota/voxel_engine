# Dear Diary, here are the moments I wanted to end my suffering..

1. [6. 2. 2025] 
- AABB collisions (again) CollisionInfo.axis, I dont know binary apparently. I was storing 3rd axis as info.axis |= 3 while it should be info.axis |= 4. I was phasing through block on z axis...
- It looks like it was not the only issue.. To update AABB cache I used static_cast<glm::ivec3>(pos) and negative values it floors up rather than down. glm::floor was needed and my suffering has ended!