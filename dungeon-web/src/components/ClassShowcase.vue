<script setup>
import { ref, computed } from 'vue'

const classes = [
  {
    id: 'warrior',
    emoji: '🛡️',
    name: '誓约骑士',
    role: '近战 · 坦克',
    desc: '以血肉之躯筑起防线。反击越凶猛，护盾越坚固。',
    stats: { 攻击: 70, 防御: 95, 敏捷: 40 }
  },
  {
    id: 'mage',
    emoji: '🔥',
    name: '余烬法师',
    role: '远程 · 爆发',
    desc: '操控被禁忌的烈焰，瞬间将成群敌人化为灰烬。',
    stats: { 攻击: 95, 防御: 30, 敏捷: 55 }
  },
  {
    id: 'rogue',
    emoji: '🗡️',
    name: '暗影游侠',
    role: '敏捷 · 刺杀',
    desc: '在阴影间穿梭，一击致命后消失得无影无踪。',
    stats: { 攻击: 80, 防御: 45, 敏捷: 98 }
  }
]

const activeId = ref(classes[0].id)
const active = computed(() => classes.find((c) => c.id === activeId.value))
</script>

<template>
  <section id="classes" class="classes">
    <header class="section-head">
      <p class="eyebrow">选择命运</p>
      <h2>谁将深入地牢</h2>
    </header>

    <div class="showcase">
      <div class="tabs">
        <button
          v-for="c in classes"
          :key="c.id"
          :class="['tab', { active: c.id === activeId }]"
          @click="activeId = c.id"
        >
          <span class="tab-emoji">{{ c.emoji }}</span>
          <span>{{ c.name }}</span>
        </button>
      </div>

      <transition name="fade" mode="out-in">
        <div class="detail" :key="active.id">
          <div class="portrait">{{ active.emoji }}</div>
          <div class="info">
            <h3>{{ active.name }}</h3>
            <span class="role">{{ active.role }}</span>
            <p>{{ active.desc }}</p>
            <ul class="bars">
              <li v-for="(val, key) in active.stats" :key="key">
                <span class="bar-label">{{ key }}</span>
                <span class="bar-track">
                  <span class="bar-fill" :style="{ width: val + '%' }"></span>
                </span>
                <span class="bar-val">{{ val }}</span>
              </li>
            </ul>
          </div>
        </div>
      </transition>
    </div>
  </section>
</template>

<style scoped>
.classes {
  padding: 120px 6vw;
  background: var(--bg-soft);
}
.section-head {
  text-align: center;
  margin-bottom: 56px;
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
.showcase {
  max-width: 900px;
  margin: 0 auto;
}
.tabs {
  display: flex;
  gap: 12px;
  justify-content: center;
  flex-wrap: wrap;
  margin-bottom: 36px;
}
.tab {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 12px 22px;
  background: var(--panel);
  border: 1px solid var(--border);
  border-radius: 10px;
  color: var(--muted);
  font-weight: 600;
  transition: all 0.2s;
}
.tab:hover {
  color: var(--text);
  border-color: var(--accent);
}
.tab.active {
  color: #1a1208;
  background: linear-gradient(120deg, var(--accent-bright), var(--accent));
  border-color: transparent;
}
.tab-emoji {
  font-size: 1.2rem;
}
.detail {
  display: grid;
  grid-template-columns: 220px 1fr;
  gap: 36px;
  align-items: center;
  background: var(--panel);
  border: 1px solid var(--border);
  border-radius: 18px;
  padding: 40px;
}
.portrait {
  font-size: 7rem;
  text-align: center;
  background: radial-gradient(circle, rgba(244, 211, 94, 0.15), transparent 70%);
  border-radius: 16px;
  padding: 24px 0;
}
.info h3 {
  font-size: 1.8rem;
  color: var(--accent-bright);
}
.role {
  display: inline-block;
  font-size: 0.8rem;
  letter-spacing: 2px;
  color: var(--blood);
  text-transform: uppercase;
  margin: 6px 0 14px;
}
.info p {
  color: var(--muted);
  margin-bottom: 22px;
}
.bars {
  list-style: none;
  display: flex;
  flex-direction: column;
  gap: 12px;
}
.bars li {
  display: grid;
  grid-template-columns: 48px 1fr 36px;
  align-items: center;
  gap: 12px;
}
.bar-label {
  font-size: 0.85rem;
  color: var(--muted);
}
.bar-track {
  height: 8px;
  background: var(--bg);
  border-radius: 4px;
  overflow: hidden;
}
.bar-fill {
  display: block;
  height: 100%;
  background: linear-gradient(90deg, var(--accent), var(--blood));
  border-radius: 4px;
  transition: width 0.5s ease;
}
.bar-val {
  font-size: 0.85rem;
  text-align: right;
  color: var(--accent-bright);
}

.fade-enter-active,
.fade-leave-active {
  transition: opacity 0.25s, transform 0.25s;
}
.fade-enter-from,
.fade-leave-to {
  opacity: 0;
  transform: translateY(12px);
}

@media (max-width: 640px) {
  .detail {
    grid-template-columns: 1fr;
    text-align: center;
    padding: 28px;
  }
  .bars li {
    text-align: left;
  }
}
</style>
