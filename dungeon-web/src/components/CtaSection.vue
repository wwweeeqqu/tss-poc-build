<script setup>
import { ref, computed } from 'vue'

const email = ref('')
const submitted = ref(false)

const valid = computed(() => /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email.value))

function subscribe() {
  if (!valid.value) return
  submitted.value = true
}
</script>

<template>
  <section id="cta" class="cta">
    <div class="glow"></div>
    <div class="cta-inner">
      <p class="eyebrow">2026 即将降临</p>
      <h2>成为最先坠入深渊的人</h2>
      <p class="lead">
        登记你的传令鸦，第一时间获取封测资格、独家剧情与限定皮肤。
      </p>

      <form v-if="!submitted" class="signup" @submit.prevent="subscribe">
        <input
          v-model="email"
          type="email"
          placeholder="输入你的邮箱地址"
          aria-label="邮箱地址"
        />
        <button type="submit" :disabled="!valid">领取封测资格</button>
      </form>

      <p v-else class="thanks">
        ✅ 火把已点燃，{{ email }} —— 地牢之门为你敞开。
      </p>

      <div class="platforms">
        <span>登陆平台：</span>
        <span class="badge">🪟 PC</span>
        <span class="badge">🎮 主机</span>
        <span class="badge">☁️ 云游戏</span>
      </div>
    </div>
  </section>
</template>

<style scoped>
.cta {
  position: relative;
  padding: 130px 6vw;
  text-align: center;
  background:
    radial-gradient(circle at 50% 0%, rgba(139, 30, 63, 0.4), transparent 60%),
    var(--bg);
  overflow: hidden;
}
.glow {
  position: absolute;
  top: -40%;
  left: 50%;
  transform: translateX(-50%);
  width: 600px;
  height: 600px;
  background: radial-gradient(circle, rgba(244, 211, 94, 0.12), transparent 70%);
  pointer-events: none;
}
.cta-inner {
  position: relative;
  z-index: 2;
  max-width: 620px;
  margin: 0 auto;
}
.eyebrow {
  text-transform: uppercase;
  letter-spacing: 4px;
  color: var(--accent);
  font-size: 0.85rem;
  margin-bottom: 12px;
}
h2 {
  font-size: clamp(2rem, 5vw, 3.2rem);
  font-weight: 800;
  letter-spacing: 1px;
}
.lead {
  color: var(--muted);
  margin: 18px 0 36px;
  font-size: 1.1rem;
}
.signup {
  display: flex;
  gap: 12px;
  max-width: 460px;
  margin: 0 auto;
  flex-wrap: wrap;
  justify-content: center;
}
.signup input {
  flex: 1;
  min-width: 220px;
  padding: 14px 18px;
  background: var(--panel);
  border: 1px solid var(--border);
  border-radius: 8px;
  color: var(--text);
  font-size: 1rem;
}
.signup input:focus {
  outline: none;
  border-color: var(--accent);
}
.signup button {
  padding: 14px 28px;
  border: none;
  border-radius: 8px;
  font-weight: 700;
  color: #fff;
  background: linear-gradient(120deg, var(--accent), var(--blood));
  box-shadow: 0 8px 24px rgba(139, 30, 63, 0.5);
  transition: transform 0.2s, opacity 0.2s;
}
.signup button:disabled {
  opacity: 0.45;
  cursor: not-allowed;
}
.signup button:not(:disabled):hover {
  transform: translateY(-2px);
}
.thanks {
  font-size: 1.15rem;
  color: var(--accent-bright);
  padding: 18px;
  border: 1px solid var(--border);
  border-radius: 10px;
  background: var(--panel);
  max-width: 460px;
  margin: 0 auto;
}
.platforms {
  display: flex;
  gap: 12px;
  justify-content: center;
  align-items: center;
  flex-wrap: wrap;
  margin-top: 40px;
  color: var(--muted);
  font-size: 0.9rem;
}
.badge {
  padding: 6px 14px;
  background: var(--panel);
  border: 1px solid var(--border);
  border-radius: 20px;
  color: var(--text);
}
</style>
