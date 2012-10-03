#include "foldercontroller.h"
#include "library_window.h"  //get libraries

#include "folder.h"

#include "template.h"
#include "../static.h"

#include "qnaturalsorting.h"

struct LibraryItemSorter
{
	bool operator()(const LibraryItem * a,const LibraryItem * b) const
	{
		return naturalSortLessThanCI(a->name,b->name);
	} 
};

extern LibraryWindow * mw;

FolderController::FolderController() {}

void FolderController::service(HttpRequest& request, HttpResponse& response)
{
	response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");

	HttpSession session=Static::sessionStore->getSession(request,response);

	QString y = session.get("xxx").toString();
	//response.writeText(QString("session xxx : %1 <br/>").arg(y));

	Template t=Static::templateLoader->getTemplate("folder_"+session.getDeviceType(),request.getHeader("Accept-Language"));
	t.enableWarnings();
	QString path = QUrl::fromPercentEncoding(request.getPath()).toLatin1();
	QStringList pathElements = path.split('/');
	QString libraryName = pathElements.at(2);
	qulonglong parentId = pathElements.at(4).toULongLong();
	QList<LibraryItem *> folderContent = mw->getFolderContentFromLibrary(libraryName,parentId);
	QList<LibraryItem *> folderComics = mw->getFolderComicsFromLibrary(libraryName,parentId);

	response.writeText(libraryName);

	folderContent.append(folderComics);

	qSort(folderContent.begin(),folderContent.end(),LibraryItemSorter());
	folderComics.clear();

	qulonglong backId = mw->getParentFromComicFolderId(libraryName,parentId);

	int page = 0;
	QByteArray p = request.getParameter("page");
	if(p.length() != 0)
		page = p.toInt();

	// /comicIdi/pagei/comicIdj/pagej/....../comicIdn/pagen
	//QString currentPath = session.get("currentPath").toString();
	//QStringList pathSize = currentPath.split("/").last().toInt;

	if(backId == 1 && parentId == 1)
		t.setVariable(QString("upurl"),"/?page=0");
	else
		t.setVariable(QString("upurl"),"/library/" + QUrl::toPercentEncoding(libraryName) + "/folder/" +QString("%1").arg(backId));//.arg(upPage));		

	/*if(currentPath.length()>0)
	{
		if(currentPath.contains(QString("%1").arg(parentId))
		{

		}
		else
		{
			session.set("currentPath",currentPath+QString("/%1/%2").arg(parentId).arg(page);
		}
	}*/


	//t.loop("element",folderContent.length());

	int elementsPerPage = 20;

	int numFolders = folderContent.length();
	int numComics = folderComics.length();
	int totalLength = folderContent.length() + folderComics.length();

	int numFolderPages = numFolders / elementsPerPage + ((numFolders%elementsPerPage)>0?1:0);
	int numPages = totalLength / elementsPerPage + ((totalLength%elementsPerPage)>0?1:0);

	//response.writeText(QString("Number of pages : %1 <br/>").arg(numPages));

	if(page < 0)
		page = 0;
	else if(page >= numPages)
		page = numPages-1;

	int indexCurrentPage = page*elementsPerPage;
	int numFoldersAtCurrentPage = qMax(0,qMin(numFolders - indexCurrentPage, elementsPerPage));

	//response.writeText(QString("indexCurrentPage : %1 <br/>").arg(indexCurrentPage));
	//response.writeText(QString("numFoldersAtCurrentPage : %1 <br/>").arg(numFoldersAtCurrentPage));
	//response.writeText(QString("foldersLength : %1 <br/>").arg(folderContent.length()));

	t.loop("element",numFoldersAtCurrentPage);
	int i = 0;
	while(i<numFoldersAtCurrentPage)
	{
		LibraryItem * item = folderContent.at(i + (page*elementsPerPage));
		t.setVariable(QString("element%1.name").arg(i),folderContent.at(i + (page*elementsPerPage))->name);
		if(item->isDir())
		{
			t.setVariable(QString("element%1.image.width").arg(i),"89px");
			t.setVariable(QString("element%1.image.url").arg(i),"/images/f.png");

			t.setVariable(QString("element%1.browse").arg(i),QString("<a href=\"%1\">Browse</a>").arg(QString("/library/%1/folder/%2").arg(libraryName).arg(item->id)));

			//t.setVariable(QString("element%1.url").arg(i),"/library/"+libraryName+"/folder/"+QString("%1").arg(folderContent.at(i + (page*10))->id));
			//t.setVariable(QString("element%1.downloadurl").arg(i),"/library/"+libraryName+"/folder/"+QString("%1/info").arg(folderContent.at(i + (page*elementsPerPage))->id));
			t.setVariable(QString("element%1.download").arg(i),QString("<a href=\"%1\">Download</a>").arg("/library/"+QUrl::toPercentEncoding(libraryName)+"/folder/"+QString("%1/info").arg(folderContent.at(i + (page*elementsPerPage))->id)));
		}
		else
		{
			const ComicDB * comic = (ComicDB *)item;
			t.setVariable(QString("element%1.browse").arg(i),"");
			t.setVariable(QString("element%1.image.width").arg(i),"80px");
			//t.setVariable(QString("element%1.downloadurl").arg(i),"/library/"+libraryName+"/comic/"+QString("%1").arg(comic->id));
			if(!session.isComicOnDevice(comic->info.hash))
				t.setVariable(QString("element%1.download").arg(i),QString("<a href=\"%1\">Download</a>").arg("/library/"+QUrl::toPercentEncoding(libraryName)+"/comic/"+QString("%1").arg(comic->id)));
			else
				t.setVariable(QString("element%1.download").arg(i),QString(""));
			
			//t.setVariable(QString("element%1.image.url").arg(i),"/images/f.png");

			t.setVariable(QString("element%1.image.url").arg(i),QString("/library/%1/cover/%2.jpg").arg(QString(QUrl::toPercentEncoding(libraryName))).arg(comic->info.hash));
		}
		i++;
	}

	int comicsOffset;// = qMax(0,((page - (numFolderPages - 1)) * 10) - (numFolders%10));

	int comicPage = numFolderPages!=0?page-(numFolderPages - 1):page;

	if(comicPage > 0)
	{
		comicsOffset = elementsPerPage - (numFolders%elementsPerPage);
		comicsOffset += (comicPage-1) *elementsPerPage;
	}
	else
		comicsOffset = 0;

	

	int globalComicsOffset = elementsPerPage - (numFolders%elementsPerPage);
	int numComicsAtCurrentPage = 0;

	if(comicPage == 0) //primera p�gina de los c�mics
			numComicsAtCurrentPage = qMin(globalComicsOffset,numComics);
		else if (page == (numPages-1)) //�ltima p�gina de los c�mics
			numComicsAtCurrentPage = elementsPerPage-globalComicsOffset + (numComics%elementsPerPage);
		else
			numComicsAtCurrentPage = elementsPerPage - numFoldersAtCurrentPage;

	if(numComics == 0)
		numComicsAtCurrentPage = 0;
	//response.writeText(QString("numComicsAtCurrentPage : %1 <br/>").arg(numComicsAtCurrentPage));
	//response.writeText(QString("comicsOffset : %1 <br/>").arg(comicsOffset));

	t.loop("elementcomic",numComicsAtCurrentPage);
	//
	int j = 0;

	while(j<numComicsAtCurrentPage)
	{
		const ComicDB * comic = (ComicDB *)folderComics.at(j+comicsOffset);
		//if(comic->info.title == 0 || comic->info.title->isEmpty())
			t.setVariable(QString("elementcomic%1.name").arg(j),comic->name);
		//else
		//	t.setVariable(QString("elementcomic%1.name").arg(i),*comic->info.title);
		t.setVariable(QString("elementcomic%1.url").arg(j),"/library/"+QUrl::toPercentEncoding(libraryName)+"/comic/"+QString("%1").arg(comic->id));
		t.setVariable(QString("elementcomic%1.coverulr").arg(j),"/library/"+QUrl::toPercentEncoding(libraryName)+"/cover/"+QString("%1").arg(comic->info.hash + ".jpg"));
		j++;
	}

	if(numPages > 1)
	{
		QMap<QString,int> indexCount;

		QString firstChar;
		int xyz = 1;
		for(QList<LibraryItem *>::const_iterator itr=folderContent.constBegin();itr!=folderContent.constEnd();itr++)
		{
			firstChar = QString((*itr)->name[0]).toUpper();
			firstChar = firstChar.normalized(QString::NormalizationForm_D).at(0);//TODO _D or _KD??
			bool ok;
			int dec = firstChar.toInt(&ok, 10);
			if(ok)
				firstChar = "#";
			//response.writeText(QString("%1 - %2 <br />").arg((*itr)->name).arg(xyz));
			if(indexCount.contains(firstChar))
				indexCount.insert(firstChar, indexCount.value(firstChar)+1);
			else
				indexCount.insert(firstChar, 1);

			xyz++;
		}

		QList<QString> index = indexCount.keys();
		qSort(index.begin(),index.end(),naturalSortLessThanCI);
		t.loop("index",index.length());
		int i=0;
		int count=0;
		int indexPage=0;
		for(QList<QString>::const_iterator itr=index.constBegin();itr!=index.constEnd();itr++)
		{
			//response.writeText(QString("%1 - %2 <br />").arg(*itr).arg(count));
			t.setVariable(QString("index%1.indexname").arg(i), *itr);
			t.setVariable(QString("index%1.url").arg(i),QString("/library/%1/folder/%2?page=%3").arg(QString(QUrl::toPercentEncoding(libraryName))).arg(parentId).arg(indexPage));
			i++;
			count += indexCount.value(*itr);
			indexPage = count/elementsPerPage;
			
		}

		t.loop("page",numPages);
		int z = 0;
		while(z < numPages)
		{

			t.setVariable(QString("page%1.url").arg(z),QString("/library/%1/folder/%2?page=%3").arg(QString(QUrl::toPercentEncoding(libraryName))).arg(parentId).arg(z));
			t.setVariable(QString("page%1.number").arg(z),QString("%1").arg(z));
			if(page == z)
				t.setVariable(QString("page%1.current").arg(z),"current");
			else
				t.setVariable(QString("page%1.current").arg(z),"");
			z++;
		}

		t.setVariable("page.first",QString("/library/%1/folder/%2?page=%3").arg(QString(QUrl::toPercentEncoding(libraryName))).arg(parentId).arg(0));
		t.setVariable("page.previous",QString("/library/%1/folder/%2?page=%3").arg(QString(QUrl::toPercentEncoding(libraryName))).arg(parentId).arg((page==0)?page:page-1));
		t.setVariable("page.next",QString("/library/%1/folder/%2?page=%3").arg(QString(QUrl::toPercentEncoding(libraryName))).arg(parentId).arg((page==numPages-1)?page:page+1));
		t.setVariable("page.last",QString("/library/%1/folder/%2?page=%3").arg(QString(QUrl::toPercentEncoding(libraryName))).arg(parentId).arg(numPages-1));
	}
	else
	{
		t.loop("page",0);
		t.loop("index",0);
	}

	response.write(t.toLatin1(),true);

}