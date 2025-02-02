<template>
    <div class="container">
        <div class="content">
            <h1>Upcoming Feature</h1>
            <p>In the next update, you will be able to manage your links directly from your user account. Stay tuned!</p>
        </div>
    </div>
</template>

<script>
export default {
    setup() {}
}
</script>

<style scoped>
.container {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 90vh; /* Full viewport height */
    text-align: center;
    padding: 0 20px; /* Adds some padding for small screens */
}

.content {
    max-width: 600px; /* Limit the width of the content */
}

p {
    font-size: 1.2em; /* Adjust font size if necessary */
    line-height: 1.4em; /* Adjust line height for better readability */
    max-width: 500px; /* Ensure the text stays within two lines */
    margin: 0 auto; /* Center the paragraph */
}

@media screen and (max-width: 768px) {
    .container {
        padding: 0 10px; /* Adjust padding for very small screens */
    }

    p {
        font-size: 1em; /* Adjust font size for smaller screens */
        max-width: 100%; /* Allow full width on small screens */
    }
}
</style>