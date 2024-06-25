addEventListener('fetch', event => {
	event.respondWith(handleRequest(event.request))
})

async function handleRequest(request) {
	const response = await fetch(request)
	const newHeaders = new Headers(response.headers)

	newHeaders.set('Cross-Origin-Opener-Policy', 'same-origin')
	newHeaders.set('Cross-Origin-Embedder-Policy', 'require-corp')

	return new Response(response.body, {
		status: response.status,
		statusText: response.statusText,
		headers: newHeaders
	})
}