<script setup>
const year = new Date().getFullYear()
const socials = [
  { label: 'Discord', icon: '💬' },
  { label: 'YouTube', icon: '▶️' },
  { label: 'X', icon: '✖️' },
  { label: 'Steam', icon: '🎮' }
]
</script>

<template>
  <footer class="site-footer">
    <div class="foot-top">
      <div class="foot-brand">
        <span class="brand-icon">🗡️</span>
        <span>暗影地牢</span>
      </div>
      <div class="socials">
        <a v-for="s in socials" :key="s.label" href="#" :title="s.label">
          <span>{{ s.icon }}</span>
        </a>
      </div>
    </div>
    <hr />
    <p class="copy">
      © {{ year }} 暗影地牢工作室 · 本页面为 Vue 3 演示作品，游戏内容纯属虚构。
    </p>
  </footer>
</template>

<style scoped>
.site-footer {
  padding: 48px 6vw 36px;
  background: var(--bg);
  border-top: 1px solid var(--border);
}
.foot-top {
  display: flex;
  align-items: center;
  justify-content: space-between;
  flex-wrap: wrap;
  gap: 20px;
}
.foot-brand {
  display: flex;
  align-items: center;
  gap: 10px;
  font-weight: 700;
  font-size: 1.1rem;
  letter-spacing: 1px;
}
.brand-icon {
  filter: drop-shadow(0 0 6px var(--accent));
}
.socials {
  display: flex;
  gap: 14px;
}
.socials a {
  width: 42px;
  height: 42px;
  display: grid;
  place-items: center;
  background: var(--panel);
  border: 1px solid var(--border);
  border-radius: 50%;
  font-size: 1.1rem;
  transition: transform 0.2s, border-color 0.2s;
}
.socials a:hover {
  transform: translateY(-3px);
  border-color: var(--accent);
}
hr {
  border: none;
  border-top: 1px solid var(--border);
  margin: 26px 0 18px;
}
.copy {
  text-align: center;
  color: var(--muted);
  font-size: 0.85rem;
}
</style>
