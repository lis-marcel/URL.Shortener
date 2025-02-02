<template>
    <div class="container">
        <div class="input">
            <form @submit.prevent="submitLink">
                <h2>Paste the URL to be shortened</h2>
                <div class="form-group mx-sm-3 mb-2">
                    <input v-model="Url" type="text" class="form-control" placeholder="Enter link here">
                    <button type="submit" class="btn btn-primary mb-2">Shorten URL</button>
                </div>
                <!-- Error message display -->
                <div v-if="errorMessage" class="error-message">{{ errorMessage }}</div>
            </form>
        </div>
        <br>
        <div v-if="shortLink" class="output">
            <div class="output-content">
                <h2>Your shortened link is:</h2>
                <p>{{ shortLink }}</p>
            </div>
            <button class="btn btn-primary mb-2" :class="{ 'btn-copied': isCopied }" @click="copyToClipboard">
                {{ isCopied ? 'Copied!' : 'Copy' }}
            </button>
        </div>
    </div>
</template>

<script>
    import { ref } from 'vue';
    import axios from 'axios';
    import { API_APP_SHORTEN } from '../config/consts';

    export default {
        setup() {
            const Url = ref('');
            const shortLink = ref('');
            const isCopied = ref(false);
            const errorMessage = ref(''); // Define a ref for storing the error message

            const submitLink = async () => {
                if (!isValidUrl(Url.value)) {
                    errorMessage.value = 'Please enter a valid URL.';
                    return;
                }

                try {
                    const response = await axios.post(API_APP_SHORTEN, {
                        url: Url.value,
                    }, {
                        headers: {
                            'Content-Type': 'application/json',
                        },
                    });
                    console.log(response.data);
                    shortLink.value = response.data.shortUrl;
                    errorMessage.value = ''; // Clear error message on successful request
                } catch (error) {
                    console.error(error);
                    if (error.response && error.response.status === 400) {
                        // Assuming the server sends back a plain text message
                        errorMessage.value = error.response.data || 'An error occurred';
                    } else {
                        // Handle other types of errors (e.g., network error, server error)
                        errorMessage.value = 'An unexpected error occurred';
                    }
                }
            };

            // URL validation function
            const isValidUrl = (url) => {
                const urlPattern = new RegExp('^(https?:\\/\\/)?'+ // protocol
                    '((([a-z\\d]([a-z\\d-]*[a-z\\d])*)\\.)+[a-z]{2,}|'+ // domain name
                    '((\\d{1,3}\\.){3}\\d{1,3}))'+ // OR ip (v4) address
                    '(\\:\\d+)?(\\/[-a-z\\d%_.~+]*)*'+ // port and path
                    '(\\?[;&a-z\\d%_.~+=-]*)?'+ // query string
                    '(\\#[-a-z\\d_]*)?$','i'); // fragment locator
                try {
                    return !!new URL(url) && urlPattern.test(url);
                } catch (_) {
                    return false;
                }
            };

            const copyToClipboard = () => {
                navigator.clipboard.writeText(shortLink.value);

                isCopied.value = true;

                setTimeout(() => {
                    isCopied.value = false;
                }, 3000);
            };

            return {
                Url,
                shortLink,
                submitLink,
                copyToClipboard,
                isCopied,
                errorMessage, // Make errorMessage available to the template
            };
        },
    };
</script>

<style scoped>
    .container {
        margin: 5vh auto; /* Center the container horizontally */
        display: flex;
        justify-content: center;
        align-items: center;
        flex-direction: column;
        min-height: 35vh; /* Set a minimum height */
        width: 80%; /* Make width responsive */
        max-width: 600px; /* Set a maximum width */
        min-width: 300px; /* Set a minimum width to ensure content fits */
        border: 1px solid #ccc;
        border-radius: 5px;
        box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
    }

    .form-group {
        display: flex;
        align-items: center;
    }

    .form-control {
        margin-right: 10px;
    }

    .btn-primary {
        margin-left: 10px;
    }

    .output {
        text-align: center;
        margin-top: 20px;
        display: flex;
        flex-direction: column;
        align-items: center;
    }

    .btn-copied, .btn-copied:hover {
        background-color: green;
        color: white;
    }

    .output-content {
        margin-bottom: 10px;
    }

    .error-message {
        background-color: #f8d7da; /* Light red background */
        color: #721c24; /* Dark red text color for contrast */
        padding: 10px;
        margin-top: 10px;
        border-radius: 5px;
        border: 1px solid #f5c6cb;
    }

    @media (max-width: 768px) {
        .container {
            width: 90%; /* Increase width for smaller screens */
        }
    }

    @media (max-width: 480px) {
        .container {
            flex-direction: column;
            width: 95%; /* Further increase width for very small screens */
        }

        .form-group {
            flex-direction: column; /* Stack input and button vertically on small screens */
            align-items: stretch;
        }

        .form-control, .btn-primary {
            width: 100%; /* Make input and button take full width */
            margin: 10px 0; /* Add some margin for spacing */
        }
    }
</style>