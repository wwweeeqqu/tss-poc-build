<script setup>
import { ref, onMounted } from 'vue'

const stats = [
  { value: '120+', label: '随机地牢层' },
  { value: '60', label: '可解锁技能' },
  { value: '∞', label: '永久死亡冒险' }
]

const torch = ref(false)
onMounted(() => {
  torch.value = true
})
</script>

<template>
  <section id="top" class="hero">
    <div class="vignette"></div>
    <div class="embers">
      <span v-for="n in 18" :key="n" :style="{ '--i': n }"></span>
    </div>

    <div :class="['hero-content', { lit: torch }]">
      <p class="eyebrow">深渊之下 · 无人生还</p>
      <h1>暗影地牢</h1>
      <p class="tagline">
        点亮你最后一根火把，踏入永远在变化的地牢。
        击败潜伏在黑暗中的古神眷属，或成为它们的下一顿盛宴。
      </p>

      <div class="cta-row">
        <a href="#cta" class="btn btn-primary">立即出征</a>
        <a href="#features" class="btn btn-ghost">▶ 观看预告</a>
      </div>

      <div class="stats">
        <div v-for="s in stats" :key="s.label" class="stat">
          <span class="stat-value">{{ s.value }}</span>
          <span class="stat-label">{{ s.label }}</span>
        </div>
      </div>
    </div>

    <span class="scroll-hint">向下滚动探索 ▾</span>
  </section>
</template>

<style scoped>
.hero {
  position: relative;
  min-height: 100vh;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  text-align: center;
  padding: 120px 6vw 80px;
  background:
    radial-gradient(circle at 50% 30%, #2a1a3a 0%, transparent 55%),
    radial-gradient(circle at 50% 120%, var(--blood) 0%, transparent 50%),
    var(--bg);
  overflow: hidden;
}
.vignette {
  position: absolute;
  inset: 0;
  background: radial-gradient(circle at 50% 45%, transparent 30%, rgba(0, 0, 0, 0.85) 100%);
  pointer-events: none;
}
.hero-content {
  position: relative;
  z-index: 2;
  max-width: 760px;
  opacity: 0;
  transform: translateY(24px);
  transition: opacity 1s ease, transform 1s ease;
}
.hero-content.lit {
  opacity: 1;
  transform: translateY(0);
}
.eyebrow {
  text-transform: uppercase;
  letter-spacing: 4px;
  color: var(--accent);
  font-size: 0.85rem;
  margin-bottom: 14px;
}
h1 {
  font-size: clamp(3rem, 10vw, 6.5rem);
  font-weight: 800;
  letter-spacing: 4px;
  line-height: 1.05;
  background: linear-gradient(180deg, #fff 0%, var(--accent-bright) 60%, var(--blood) 100%);
  -webkit-background-clip: text;
  background-clip: text;
  color: transparent;
  text-shadow: 0 0 40px rgba(244, 211, 94, 0.25);
}
.tagline {
  margin: 22px auto 0;
  max-width: 560px;
  color: var(--muted);
  font-size: 1.1rem;
}
.cta-row {
  display: flex;
  gap: 16px;
  justify-content: center;
  flex-wrap: wrap;
  margin-top: 36px;
}
.btn {
  padding: 14px 32px;
  border-radius: 8px;
  font-weight: 600;
  letter-spacing: 1px;
  transition: transform 0.2s, box-shadow 0.2s, background 0.2s;
}
.btn-primary {
  background: linear-gradient(120deg, var(--accent), var(--blood));
  color: #fff;
  box-shadow: 0 8px 28px rgba(139, 30, 63, 0.5);
}
.btn-primary:hover {
  transform: translateY(-3px);
  box-shadow: 0 12px 34px rgba(139, 30, 63, 0.7);
}
.btn-ghost {
  border: 1px solid var(--border);
  color: var(--text);
}
.btn-ghost:hover {
  border-color: var(--accent);
  color: var(--accent-bright);
  transform: translateY(-3px);
}
.stats {
  display: flex;
  gap: 48px;
  justify-content: center;
  flex-wrap: wrap;
  margin-top: 56px;
}
.stat {
  display: flex;
  flex-direction: column;
}
.stat-value {
  font-size: 2.2rem;
  font-weight: 800;
  color: var(--accent-bright);
}
.stat-label {
  font-size: 0.85rem;
  color: var(--muted);
  letter-spacing: 1px;
}
.scroll-hint {
  position: absolute;
  bottom: 28px;
  z-index: 2;
  color: var(--muted);
  font-size: 0.8rem;
  letter-spacing: 2px;
  animation: bob 2s ease-in-out infinite;
}

/* floating embers */
.embers {
  position: absolute;
  inset: 0;
  z-index: 1;
  pointer-events: none;
}
.embers span {
  position: absolute;
  bottom: -10px;
  left: calc(var(--i) * 5.5%);
  width: 4px;
  height: 4px;
  background: var(--accent-bright);
  border-radius: 50%;
  opacity: 0;
  filter: blur(0.5px);
  box-shadow: 0 0 8px var(--accent);
  animation: rise calc(6s + var(--i) * 0.4s) linear infinite;
  animation-delay: calc(var(--i) * -0.6s);
}

@keyframes rise {
  0% { transform: translateY(0) scale(1); opacity: 0; }
  15% { opacity: 0.9; }
  100% { transform: translateY(-90vh) scale(0.3); opacity: 0; }
}
@keyframes bob {
  0%, 100% { transform: translateY(0); }
  50% { transform: translateY(8px); }
}
</style>
