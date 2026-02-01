---
name: jira-default
description: Interact with Jira for Data Science projects to create, list, view, edit, and transition issues. Use when the user asks about Jira tasks, tickets, or issues but didn't mention about the project.
license: MIT
compatibility: Requires access to Atlassian MCP tools.
metadata:
  author: GitHub Copilot
  version: "1.0"
---

# Jira Default Project

If user did not mention about the project in Atlassian Jira, assume it to be Data Science project.

## When to Use

- User requested to interact with jira related items but didn't specify the project, assume it to be Data Science project.
