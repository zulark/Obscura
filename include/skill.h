#ifndef SKILL_H
#define SKILL_H

#define MAX_SKILLS 10

typedef struct  Skill{
    char name[32];
    int level;
    bool unlocked;
    
} Skill;

typedef struct SkillTree {
    Skill skills[MAX_SKILLS]; // Exemplo: 10 habilidades
    int skillPoints;
} SkillTree;

#endif