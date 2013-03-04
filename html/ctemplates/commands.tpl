{{! -*-HTML-*- }}
{{>HEADER}}
<body>
	{{>PAGE_HEADER}}
		<ul>
		{{#HANDLER}}
			<li><a href="{{name}}">{{name}}</a> 
			<p>{{description}}</p></li>
		{{/HANDLER}}
		</ul>
	{{>PAGE_FOOTER}}
</body>
{{>FOOTER}}
