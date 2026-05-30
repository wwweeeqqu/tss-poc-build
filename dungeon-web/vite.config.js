import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'

export default defineConfig({
  // 部署在 GitHub Pages 项目站点的子路径下：https://<user>.github.io/tss-poc-build/
  base: '/tss-poc-build/',
  plugins: [vue()]
})
