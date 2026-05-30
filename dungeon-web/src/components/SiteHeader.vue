<script setup>
import { ref, onMounted, onUnmounted } from 'vue'

const scrolled = ref(false)
const menuOpen = ref(false)

const links = [
  { label: '特色', href: '#features' },
  { label: '英雄', href: '#classes' },
  { label: '加入', href: '#cta' }
]

function onScroll() {
  scrolled.value = window.scrollY > 40
}

onMounted(() => window.addEventListener('scroll', onScroll))
onUnmounted(() => window.removeEventListener('scroll', onScroll))
</script>

<template>
  <header :class="['site-header', { scrolled }]">
    <a class="brand" href="#top">
      <span class="brand-icon">🗡️</span>
      <span class="brand-name">暗影地牢</span>
    </a>

    <nav :class="{ open: menuOpen }">
      <a
        v-for="link in links"
        :key="link.href"
        :href="link.href"
        @click="menuOpen = false"
      >{{ link.label }}</a>
    </nav>

    <button class="burger" @click="menuOpen = !menuOpen" aria-label="菜单">
      <span></span><span></span><span></span>
    </button>
  </header>
</template>

<style scoped>
.site-header {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  z-index: 100;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 18px 6vw;
  transition: background 0.3s, padding 0.3s, box-shadow 0.3s;
}
.site-header.scrolled {
  background: rgba(13, 11, 20, 0.85);
  backdrop-filter: blur(10px);
  padding: 12px 6vw;
  box-shadow: 0 2px 20px rgba(0, 0, 0, 0.5);
}
.brand {
  display: flex;
  align-items: center;
  gap: 10px;
  font-weight: 700;
  font-size: 1.25rem;
  letter-spacing: 1px;
}
.brand-icon {
  font-size: 1.5rem;
  filter: drop-shadow(0 0 6px var(--accent));
}
.brand-name {
  background: linear-gradient(120deg, var(--accent-bright), var(--blood));
  -webkit-background-clip: text;
  background-clip: text;
  color: transparent;
}
nav {
  display: flex;
  gap: 32px;
}
nav a {
  position: relative;
  color: var(--muted);
  font-weight: 500;
  transition: color 0.25s;
}
nav a:hover {
  color: var(--accent-bright);
}
nav a::after {
  content: "";
  position: absolute;
  left: 0;
  bottom: -6px;
  width: 0;
  height: 2px;
  background: var(--accent);
  transition: width 0.25s;
}
nav a:hover::after {
  width: 100%;
}
.burger {
  display: none;
  flex-direction: column;
  gap: 5px;
  background: none;
  border: none;
  padding: 6px;
}
.burger span {
  width: 26px;
  height: 2px;
  background: var(--text);
  border-radius: 2px;
}

@media (max-width: 720px) {
  nav {
    position: absolute;
    top: 100%;
    right: 6vw;
    flex-direction: column;
    gap: 16px;
    background: var(--panel);
    border: 1px solid var(--border);
    border-radius: 12px;
    padding: 20px 28px;
    transform: translateY(-10px);
    opacity: 0;
    pointer-events: none;
    transition: opacity 0.2s, transform 0.2s;
  }
  nav.open {
    opacity: 1;
    transform: translateY(0);
    pointer-events: auto;
  }
  .burger {
    display: flex;
  }
}
</style>
