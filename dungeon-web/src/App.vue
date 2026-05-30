<script setup>
import SiteHeader from './components/SiteHeader.vue'
import HeroSection from './components/HeroSection.vue'
import FeatureGrid from './components/FeatureGrid.vue'
import ClassShowcase from './components/ClassShowcase.vue'
import CtaSection from './components/CtaSection.vue'
import SiteFooter from './components/SiteFooter.vue'
</script>

<template>
  <SiteHeader />
  <main>
    <HeroSection />
    <FeatureGrid />
    <ClassShowcase />
    <CtaSection />
  </main>
  <SiteFooter />
</template>
