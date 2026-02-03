from flask import Flask, request, jsonify
from autocomplete import autocomplete, init_tree, get_trie


app = Flask(__name__)


@app.before_request
def startup():
    init_tree("./corpuses/trycorpus.he")


@app.route('/')
def home():
    """Home page with API docs"""
    return """
    <h1>Autocomplete API</h1>
    <p>Endpoints:</p>
    <ul>
        <li><code>GET /autocomplete?q=query&limit=5</code> - Get suggestions</li>
        <li><code>GET /health</code> - Health check</li>
        <li><code>GET /stats</code> - Server statistics</li>
    </ul>
    <p>Example: <a href="/autocomplete?q=hel&limit=5">/autocomplete?q=hel&limit=5</a></p>
    """



@app.route("/health", methods=["GET"])
def health():
    tr = get_trie()
    return jsonify({
        "trie": tr is not None, 
        "running": True 
    }),200

@app.route("/autocomplete", methods=["GET"])
def autocomplete_serv():
    q = request.args.get("q", "").strip()
    k = int(request.args.get("k", 5))

    if not q:
        return jsonify({"error": "Missing q"}), 400
    results = autocomplete(q, k)
    
    return jsonify({
        "query": q,
        "k": k,
        "results": results
    })



if __name__ == "__main__":
    init_tree("./corpuses/trycorpus.he")
    print("Server started on the port", 8000)
    app.run(host="0.0.0.0", port=8000, threaded=True)