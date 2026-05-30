# 暗影地牢 · Shadow Dungeon

一个基于 **Vue 3 + Vite** 的地牢游戏宣传单页（Landing Page）演示。

## 预览内容

- 🗡️ **固定导航栏** — 滚动时半透明吸顶，移动端汉堡菜单
- 🔥 **英雄首屏** — 渐变标题、漂浮火星动画、数据统计
- 🌀 **特色网格** — 卡片式展示游戏核心玩法
- 🛡️ **职业展示** — 可切换标签 + 属性条（Vue 响应式 + 过渡动画）
- 📨 **封测登记** — 带邮箱校验的表单交互
- 底部社交链接与版权信息

## 技术栈

- [Vue 3](https://vuejs.org/)（`<script setup>` 组合式 API）
- [Vite](https://vitejs.dev/) 构建工具
- 纯 CSS（Scoped Style，无 UI 框架依赖）

## 本地运行

```bash
cd dungeon-web
npm install      # 安装依赖
npm run dev      # 启动开发服务器（默认 http://localhost:5173）
```

## 构建生产版本

```bash
npm run build    # 输出到 dist/
npm run preview  # 本地预览构建结果
```

## 目录结构

```
dungeon-web/
├── index.html
├── vite.config.js
├── package.json
└── src/
    ├── main.js
    ├── styles.css          # 全局变量与基础样式
    ├── App.vue             # 根组件
    └── components/
        ├── SiteHeader.vue
        ├── HeroSection.vue
        ├── FeatureGrid.vue
        ├── ClassShowcase.vue
        ├── CtaSection.vue
        └── SiteFooter.vue
```

> 本页面仅为前端演示作品，游戏内容纯属虚构。
