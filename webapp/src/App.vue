<script setup>
import Navbar from './components/Navbar.vue'
import { RouterView } from 'vue-router'
</script>

<template>
  <Navbar />
  <RouterView />
</template>

<style>
  body {
    transition: background-color 0.3s, color 0.3s;
  }

  .dark-mode {
    background-color: #121212;
    color: #ffffff;
  }

  .dark-mode .navbar {
    background-color: #333333;
  }

  .dark-mode .navbar-nav li.nav-item:hover a {
    background-color: #444444; /* Slightly lighter grey for hover */
  }

  /* Add any other global styles for dark mode here */

</style>