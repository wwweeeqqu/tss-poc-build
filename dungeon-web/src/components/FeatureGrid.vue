<script setup>
const features = [
  {
    icon: '🌀',
    title: '程序生成地牢',
    desc: '每一次下潜都是全新的迷宫布局、陷阱与宝藏，没有两局相同的冒险。'
  },
  {
    icon: '⚔️',
    title: '硬核实时战斗',
    desc: '闪避、格挡、连招与处决。读懂敌人的攻击意图，否则在黑暗中倒下。'
  },
  {
    icon: '💀',
    title: '永久死亡',
    desc: '死亡是真实的代价。但你的传承会留存，让下一位冒险者走得更远。'
  },
  {
    icon: '🔮',
    title: '诅咒与神器',
    desc: '数百种可叠加的神器构筑流派，每件力量都伴随着一丝疯狂的代价。'
  },
  {
    icon: '🐉',
    title: '史诗 BOSS',
    desc: '古神的眷属盘踞在每一层深渊，多阶段战斗考验你的极限操作。'
  },
  {
    icon: '🤝',
    title: '双人合作',
    desc: '邀请挚友并肩作战，共享战利品，也共担葬身地牢的风险。'
  }
]
</script>

<template>
  <section id="features" class="features">
    <header class="section-head">
      <p class="eyebrow">为何踏入</p>
      <h2>黑暗在召唤</h2>
    </header>

    <div class="grid">
      <article v-for="f in features" :key="f.title" class="card">
        <div class="card-icon">{{ f.icon }}</div>
        <h3>{{ f.title }}</h3>
        <p>{{ f.desc }}</p>
      </article>
    </div>
  </section>
</template>

<style scoped>
.features {
  padding: 120px 6vw;
  background: linear-gradient(180deg, var(--bg) 0%, var(--bg-soft) 100%);
}
.section-head {
  text-align: center;
  margin-bottom: 64px;
}
.eyebrow {
  text-transform: uppercase;
  letter-spacing: 4px;
  color: var(--accent);
  font-size: 0.85rem;
  margin-bottom: 10px;
}
h2 {
  font-size: clamp(2rem, 5vw, 3rem);
  font-weight: 800;
  letter-spacing: 2px;
}
.grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
  gap: 24px;
  max-width: 1100px;
  margin: 0 auto;
}
.card {
  background: var(--panel);
  border: 1px solid var(--border);
  border-radius: 14px;
  padding: 32px 28px;
  transition: transform 0.25s, border-color 0.25s, box-shadow 0.25s;
}
.card:hover {
  transform: translateY(-6px);
  border-color: var(--accent);
  box-shadow: 0 16px 40px rgba(0, 0, 0, 0.5);
}
.card-icon {
  font-size: 2.4rem;
  margin-bottom: 16px;
  filter: drop-shadow(0 0 10px rgba(244, 211, 94, 0.4));
}
.card h3 {
  font-size: 1.25rem;
  margin-bottom: 10px;
  color: var(--accent-bright);
}
.card p {
  color: var(--muted);
  font-size: 0.98rem;
}
</style>
