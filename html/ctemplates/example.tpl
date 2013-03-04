{{! -*-HTML-*- }}
{{>HEADER}}
<body>
	{{>PAGE_HEADER}}
		<p>{{message}}</p>
		<form method="POST">
		  <textarea name="message"></textarea>
		  <br/>
		  <button>Submit</button>
		</form>
	{{>PAGE_FOOTER}}
</body>
{{>FOOTER}}

